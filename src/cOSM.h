#include <map>

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

    const std::vector<int> &getNodes() const
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
    cNodeOSM &getNode(int id)
    {
        return myNode[id];
    }

    cxy latlon2pixel(double lat, double lon);

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

    void calculateBBox();
    void calculatePixel();
};