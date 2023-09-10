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
        : myID(id), myLat( lat ), myLon( lon)
    {
    }
    void setPixel( const cxy& p )
    {
        myPixel = p;
    }

    void getlatlon( double& lat, double& lon ) const
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
        const cxy& point,
        const cOSM& theOSM,
         cxy& closest  );

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
    void set(
        double latOff, double latScale,
        double lonOff, double lonScale);
    void get(
        double &latOff, double &latScale,
        double &lonOff, double &lonScale) const;

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
    getNode(const int id) const
    {
        return myNode.at(id);
    }

    // Calculate bounding box for all nodes
    // Sets the offsets for coneversion to pixel location
    // to north west corner of box
    void calculateBBox();

    // Calculate pixel location from ever node's lat & lon
    void calculatePixel();

    // convert lat, lon to pixel x,y
    cxy latlon2pixel(double lat, double lon);

    // closest point on way to specified point
    cxy closestOnWay( const cxy& point);

private:
    // nodes mapped by index number
    std::map<int, cNodeOSM> myNode;

    // ways
    std::vector<cWayOSM> myVWay;

    // pixel offset
    double mylatOff, mylonOff;

    // pixel scale
    double mylatScale, mylonScale;

    bool pixelSanity(const cxy &p);


};