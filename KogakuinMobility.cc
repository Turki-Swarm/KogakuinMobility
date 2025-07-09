// Implementation of KogakuinMobility
#include "inet/mobility/single/KogakuinMobility.h"
#include "inet/common/INETMath.h"
#include "inet/mobility/single/KogakuinMotionFileCache.h"
#include <cmath>
#include <cstdio>

namespace inet {

std::vector<int> KogakuinMobility::globalVisitCount;
bool            KogakuinMobility::globalInitialized = false;

Define_Module(KogakuinMobility);

KogakuinMobility::KogakuinMobility()
    : traceFile(nullptr), currentIdx(0), nextIdx(0), speed(0), maxSpeed(0)
{
}

KogakuinMobility::~KogakuinMobility()
{
    KogakuinMotionFileCache::deleteInstance();
}

void KogakuinMobility::initialize(int stage)
{
    LineSegmentsMobilityBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        const char *fname = par("traceFile");
        speed = par("speed").doubleValue();
        double refLat = par("referenceLatitude").doubleValue();
        double refLon = par("referenceLongitude").doubleValue();
        bool useLocal = !std::isnan(refLat) && !std::isnan(refLon);
        maxSpeed = speed;

        // Load the trace file
        auto cache = KogakuinMotionFileCache::getInstance();
        traceFile = cache->getFile(fname);

        if (useLocal){
            cache->convertGeoToLocal(fname, refLat, refLon);
        }

        const auto &junctions = traceFile->getAllJunctions();
        int n = junctions.size();

        if (!globalInitialized) {
            // first node to run initialize(): set up the shared counter
            globalVisitCount.assign(n, 0);
            globalInitialized = true;
        }

        junctionCoords.clear();
        junctionCoords.reserve(n);
        adjacency.assign(n, std::vector<int>());

        auto &idxMap = traceFile->indexMap;

        for (int i = 0; i < n; ++i) {
              const auto &J = junctions[i];
              if (useLocal) {
                  junctionCoords.emplace_back(J.x, J.y);
                  for (auto &p : J.localNeighbors) {
                      auto it = idxMap.find(p);
                      if (it != idxMap.end())
                          adjacency[i].push_back(it->second);
                  }
              }
              else {
                  junctionCoords.emplace_back(J.lon, J.lat);
                  for (auto &p : J.geoNeighbors) {
                      auto it = idxMap.find(p);
                      if (it != idxMap.end())
                          adjacency[i].push_back(it->second);
                  }
              }
          }

          currentIdx = 0;
          lastPosition = junctionCoords[currentIdx];

          // mark that globally we have visited junction 0 once
          globalVisitCount[0]++;
    }
}

//void KogakuinMobility::setInitialPosition()
//{
//    lastPosition = junctionCoords[currentIdx];
//}

void KogakuinMobility::setTargetPosition()
{
    const auto &nbrs = adjacency[currentIdx];
    if (nbrs.empty()) {
        stationary = true;
        nextChange = -1;
        targetPosition = lastPosition;
        return;
    }

    // find the minimum globalVisitCount among neighbors
    int minVisits = INT_MAX;
    for (int j : nbrs)
        minVisits = std::min(minVisits, globalVisitCount[j]);

    // collect all neighbors with that minimum
    std::vector<int> candidates;
    for (int j : nbrs)
        if (globalVisitCount[j] == minVisits)
            candidates.push_back(j);

    // pick one at random (default RNG stream)
    int pick = candidates.empty() ? nbrs[0]
                                  : candidates[intuniform(0, candidates.size()-1)];

    nextIdx = pick;

    targetPosition = junctionCoords[nextIdx];
    double distance = targetPosition.distance(lastPosition);
    double dt = (speed > 0 ? distance / speed : 0.0);
    nextChange = simTime() + dt;
    stationary = false;



}

void KogakuinMobility::move()
{
    simtime_t now = simTime();

    // 1) Clamp overshoot: if we've passed nextChange, snap to the target
    if (!stationary && nextChange >= SIMTIME_ZERO && now > nextChange) {
        lastPosition = targetPosition;
        now = nextChange;
    }

    // 2) Arrival handling: exactly at the boundary
    if (!stationary && nextChange >= SIMTIME_ZERO && now == nextChange) {
        // Advance index and snap to the canonical junction coordinates
        currentIdx = nextIdx;
        lastPosition = junctionCoords[currentIdx];
        globalVisitCount[currentIdx]++;

        // Pick next target and recompute speed for the next segment
        setTargetPosition();
        lastSpeed = (targetPosition - lastPosition) / (nextChange - now).dbl();
    }


    // 3) Interpolate between updates
    else if (now > lastUpdate) {
        ASSERT(nextChange == -1 || now < nextChange);
        lastPosition += lastSpeed * (now - lastUpdate).dbl();
    }

    raiseErrorIfOutside();
}

} // namespace inet
