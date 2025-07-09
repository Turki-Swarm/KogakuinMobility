// Implementation file for KogakuinMotionFileCache
#include "inet/mobility/single/KogakuinMotionFileCache.h"
#include "inet/common/INETDefs.h"  // for cRuntimeError
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>


namespace inet {

// Initialize static instance pointer
KogakuinMotionFileCache *KogakuinMotionFileCache::inst = nullptr;


// Private constructor/destructor
KogakuinMotionFileCache::KogakuinMotionFileCache() {}
KogakuinMotionFileCache::~KogakuinMotionFileCache() {
    cache.clear();
}

void KogakuinMotionFileCache::parseFile(const std::string& filename, KogakuinMotionFile& file) {
    std::ifstream in(filename);
    if (!in.is_open())
        throw cRuntimeError("KogakuinMotionFileCache: cannot open file '%s'", filename.c_str());
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty())
            continue;
        std::istringstream ss(line);
        std::vector<double> values;
        std::string token;
        // tokenize on commas
        while (std::getline(ss, token, ',')) {
            try {
                values.push_back(std::stod(token));
            }
            catch (const std::exception&) {
                continue;
            }
        }
        if (values.size() < 2)
            continue;
        double lat = values[0];
        double lon = values[1];
        auto key = std::make_pair(lat, lon);
        int idx = file.junctions.size();
        file.junctions.push_back({lat, lon, 0.0, 0.0, {}});
        file.indexMap[key] = idx;
        // remaining pairs are neighbor coords
        for (size_t i = 2; i + 1 < values.size(); i += 2) {
            double nlat = values[i];
            double nlon = values[i+1];
            file.junctions[idx].geoNeighbors.emplace_back(nlat, nlon);
        }
    }
}

void KogakuinMotionFileCache::convertGeoToLocal(const char *filename, double refLat, double refLon) {
    auto it = cache.find(filename);
    if (it == cache.end())
        return;
    auto &file = it->second;
    const double R   = 6.371e6;
    const double d2r = M_PI/180.0;
    double phi0 = refLat * d2r;
    file.indexMap.clear();
    for (int i = 0; i < (int)file.junctions.size(); ++i) {
        auto &j = file.junctions[i];
        double dLat = (j.lat - refLat) * d2r;
        double dLon = (j.lon - refLon) * d2r;
        j.x = R * dLon * cos(phi0);
        j.y = R * dLat;
        // convert neighbors
        j.localNeighbors.clear();
        for (auto &gn : j.geoNeighbors) {
            double ndLat = (gn.first  - refLat) * d2r;
            double ndLon = (gn.second - refLon) * d2r;
            double nx = R * ndLon * cos(phi0);
            double ny = R * ndLat;
            j.localNeighbors.emplace_back(nx, ny);
        }
        // add to indexMap
        file.indexMap[{j.x, j.y}] = i;
    }
}

KogakuinMotionFileCache *KogakuinMotionFileCache::getInstance() {
    if (!inst)
        inst = new KogakuinMotionFileCache();
    return inst;
}

void KogakuinMotionFileCache::deleteInstance() {
    delete inst;
    inst = nullptr;
}

const KogakuinMotionFile *KogakuinMotionFileCache::getFile(const char *filename) {
    std::string fname(filename);
    auto it = cache.find(fname);
    if (it != cache.end())
        return &it->second;
    KogakuinMotionFile file;
    parseFile(fname, file);
    auto res = cache.emplace(fname, std::move(file));
    return &res.first->second;
}

} // namespace inet
