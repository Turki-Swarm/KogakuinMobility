#ifndef __INET_KOGAKUINMOBILITY_H
#define __INET_KOGAKUINMOBILITY_H

#include "inet/common/INETDefs.h"
#include "inet/mobility/base/LineSegmentsMobilityBase.h"
#include "inet/mobility/single/KogakuinMotionFileCache.h"

namespace inet {

/**
 * @brief Mobility model that walks through street junctions loaded from a CSV.
 * Uses fixed speed to interpolate between junctions (no teleportation).
 *
 * The CSV must be parsed by KogakuinMotionFileCache, which exposes
 * a list of Junctions (lat, lon) and their neighbors.
 * This class picks a next junction at random and moves along the line
 * segment at constant speed.
 *
 * @ingroup mobility
 */
class INET_API KogakuinMobility : public LineSegmentsMobilityBase {
  protected:
    // Parsed trace data: all junctions loaded from file
    const KogakuinMotionFile                  *traceFile;
    // Working list of junction coordinates (x=lon, y=lat)
    std::vector<Coord>                         junctionCoords;
    // Adjacency list: for each index into junctionCoords, list of neighbor indices
    std::vector<std::vector<int>>              adjacency;

    static std::vector<int> globalVisitCount;
    static bool            globalInitialized;

    // State: current and next junction indices
    int                                         currentIdx;
    int                                         nextIdx;

    // Movement parameters
    double                                      speed;       // constant speed (m/s)
    double                                      maxSpeed;    // same as speed, but kept for API

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    // Read parameters and load CSV via cache
    virtual void initialize(int stage) override;
//    // Place initially at first junction (index 0)
//    virtual void setInitialPosition() override;
    // Set targetPosition to next junction and schedule move
    virtual void setTargetPosition() override;
    // Advance position along the segment at constant speed
    virtual void move() override;

  public:
    KogakuinMobility();
    virtual ~KogakuinMobility();

    // Return configured speed
    virtual double getMaxSpeed() const override { return maxSpeed; }
};

} // namespace inet

#endif // __INET_KOGAKUINMOBILITY_H
