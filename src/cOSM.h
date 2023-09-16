#include <map>

class cOSM;

// An open street map node
class cNodeOSM
{
public:
    cNodeOSM()
        : myID(-1), myPixel(-1, -1)
    {
    }
    cNodeOSM(int id, const cxy &pixel)
        : myID(id), myPixel(pixel)
    {
    }
    cNodeOSM(int id, int x, int y)
        : myID(id), myPixel(x, y)
    {
    }

    // Construct from lat and lon
    cNodeOSM(int id, double lat, double lon)
        : myID(id), myLat(lat), myLon(lon)
    {
    }
    void setPixel(const cxy &p)
    {
        myPixel = p;
    }

    void getlatlon(double &lat, double &lon) const
    {
        lat = myLat;
        lon = myLon;
    }
    cxy getPixel() const
    {
        return myPixel;
    }

private:
    double myLat;
    double myLon;
    // pixel location
    cxy myPixel;
    int myID;
};

// An open street map way
class cWayOSM
{
public:
    std::string myName;

    // the nodes that are on the way
    std::vector<int> myVNodeID;

    cWayOSM(
        const std::string &name,
        const std::vector<int> &vNodeID)
        : myName(name), myVNodeID(vNodeID)
    {
    }

    /// @brief Least distance squared from point to a way
    /// @param[in] point specified point
    /// @param[in] theOSM open street map data
    /// @param[out] closest point on a way
    /// @return distance squared from point to closest

    double distance2(
        const cxy &point,
        const cOSM &theOSM,
        cxy &closest);

    const std::vector<int> &
    getNodes() const
    {
        return myVNodeID;
    }
};

// Open Street Map data
class cOSM
{
public:
    cOSM();

    void set(
        double latOff, double latScale,
        double lonOff, double lonScale);
    void get(
        double &latOff, double &latScale,
        double &lonOff, double &lonScale) const;

    /** @brief Set bounding box for download from OSM
    /// @param NorthLat
    /// @param WestLon
    /// @param SouthLat
    /// @param EastLon

        lattitude
            runs from south to north
            +ve in northern hemispehere

            north edge of box is greatest lat value
            south edge of box is least lat value

        longitude
            runs from west to east
            -ve in North America

            west edge of box is least ( most -ve ) value
            east edge of box is greatest value
    */
    void setBBox(
        double NorthLat,
        double WestLon,
        double SouthLat,
        double EastLon);
    void getBBox(
        double &NorthLat,
        double &WestLon,
        double &SouthLat,
        double &EastLon) const;

    /// @brief read Open Street Map data downloaded to file
    /// @param fname
    void read(const std::string &fname);

    const std::vector<cWayOSM> &
    getWays() const
    {
        return myVWay;
    }

    // get node with ID
    const cNodeOSM &
    getNode(const int id) const;

    // Calculate bounding box for all nodes
    // Sets the offsets for coneversion to pixel location
    // to north west corner of box
    void calculateBBox();

    // Calculate pixel location from ever node's lat & lon
    void calculatePixel();

    // convert lat, lon to pixel x,y
    cxy latlon2pixel(double lat, double lon);

    // closest point on way to specified point
    cxy closestOnWay(const cxy &point);

    void download();

private:
    // bounding box for OSM download
    double myNorthLat, myWestLon, mySouthLat, myEastLon;

    std::string mydownloadFname;

    // nodes mapped by index number
    std::map<int, cNodeOSM> myNode;

    // ways
    std::vector<cWayOSM> myVWay;

    // pixel offset
    double mylatOff, mylonOff;

    // pixel scale
    double mylatScale, mylonScale;

    void parse( std::istream& ss );

    bool pixelSanity(const cxy &p);

    // Distance between two points ( lat, lon ) in meters
    static double
    Haversine(
        std::pair<double, double> from,
        std::pair<double, double> to);

    void calcOffsetScale(
        double north, double west, double south, double east);

    std::string makeOverpassBBox();
};