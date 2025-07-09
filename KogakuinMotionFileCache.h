#ifndef __INET_KOGAKUINMOTIONFILECACHE_H
#define __INET_KOGAKUINMOTIONFILECACHE_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <sstream>
#include "inet/common/INETDefs.h"

namespace inet {

/**
 * Represents a custom trace-based mobility file's contents for Kogakuin.
 * Stores each junction (lat, lon, x, y) and its list of neighbor junctions.
 */
class INET_API KogakuinMotionFile {
  public:
    struct Junction {
        double lat;   // geodetic latitude
        double lon;   // geodetic longitude
        double x;     // local X coordinate (meters)
        double y;     // local Y coordinate (meters)
        std::vector<std::pair<double,double>> geoNeighbors;   // original (lat,lon)
        std::vector<std::pair<double,double>> localNeighbors; // computed (x,y)
    };

  protected:
    // All junctions parsed from the file
    std::vector<Junction> junctions;
    // Only KogakuinMotionFileCache can populate these
    friend class KogakuinMotionFileCache;

  public:
    // Map from (lat, lon) to index in 'junctions' vector
    std::map<std::pair<double,double>, int> indexMap;


    /**
     * Return pointer to Junction for given coords, or nullptr if not found
     */
    const Junction *getJunction(double lat, double lon) const {
        auto key = std::make_pair(lat, lon);
        auto it = indexMap.find(key);
        if (it == indexMap.end())
            return nullptr;
        return &junctions[it->second];
    }

    /**
     * Get all junctions in the graph
     */
    const std::vector<Junction>& getAllJunctions() const { return junctions; }
};

/**
 * Singleton to read and cache custom mobility files.  Each file becomes one
 * KogakuinMotionFile, so every module can share the same parsed data.
 */
class INET_API KogakuinMotionFileCache {
  protected:
    using FileMap = std::map<std::string, KogakuinMotionFile>;
    FileMap cache;
    static KogakuinMotionFileCache *inst;

    KogakuinMotionFileCache();
    virtual ~KogakuinMotionFileCache();
    void parseFile(const std::string& filename, KogakuinMotionFile& file);

  public:
    static KogakuinMotionFileCache *getInstance();
    static void deleteInstance();
    virtual const KogakuinMotionFile *getFile(const char *filename);

    /**
     * Convert stored geodetic lat/lon into local x/y (meters) around ref point
     */
    void convertGeoToLocal(const char *filename, double refLat, double refLon);
};

} // namespace inet

#endif // __INET_KOGAKUINMOTIONFILECACHE_H
