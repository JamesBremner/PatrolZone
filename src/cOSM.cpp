#include <cmath>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// https://github.com/yhirose/cpp-httplib
// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "cPatrolZone.h"
#include "cOSM.h"

 cOSM::cOSM()
 {
    setBBox(
        45.4,
        -75.7,
        45.39,
        -75.68    );
 }

void cOSM::set(
    double latOff, double latScale,
    double lonOff, double lonScale)
{
    mylatOff = latOff;
    mylatScale = latScale;
    mylonOff = lonOff;
    mylonScale = lonScale;
}
void cOSM::get(
    double &latOff, double &latScale,
    double &lonOff, double &lonScale) const
{
    latOff = mylatOff;
    latScale = mylatScale;
    lonOff = mylonOff;
    lonScale = mylonScale;
}

bool cOSM::pixelSanity(const cxy &p)
{
    return !(p.x < 0 || p.y < 0 || p.x > 1200 || p.y > 1200);
}

cxy cOSM::latlon2pixel(double lat, double lon)
{
    cxy p(
        (int)(mylonScale * (mylonOff + lon)),
        (int)(mylatScale * (mylatOff + lat)));
    if (!pixelSanity(p))
    {
        // std::cout
        //     << "Offsets " << mylatOff << " " << mylonOff
        //     << "\n"
        //     << lat << "," << lon
        //     << " -> " << mylonOff + lon << "," << mylatOff + lat
        //     << " -> " << p.x << "," << p.y
        //     << "\n";
    }

    // if( p.x < 1 )
    //     p.x = 1;
    // if(p.x > 1000 )
    //     p.x = 1000;
    // if( p.y < 1 )
    //     p.y = 1;
    // if( p.y > 900 )
    //     p.y = 900;
    // throw std::runtime_error(
    //     "Bad pixel location");

    return p;
}

void cOSM::read(const std::string &fname)
{

    myNode.clear();
    myVWay.clear();

    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "cOSM cannot open file");

    std::istream ss(ifs.rdbuf());
    parse( ss );
}

 void cOSM::parse( std::istream& ss ) {
    int id;
    double lat, lon;
    std::string line;

    while (getline(ss, line))
    {
        // std::cout << line << "\n";
        int plat = line.find("\"lat\":");
        int pid = line.find("\"id\":");
        if (plat >= 0)
        {
            lat = atof(line.substr(plat + 6).c_str());
        }
        else if (pid >= 0)
        {
            id = atoi(line.substr(pid + 5).c_str());
        }
        else
        {
            int p = line.find("\"lon\":");
            if (p >= 0)
            {
                lon = atof(line.substr(p + 6).c_str());

                // read all node data

                myNode.insert(std::pair<int, cNodeOSM>(
                    id,
                    cNodeOSM(
                        id, lat, lon)));
            }
        }
    }

    // rewind
    ss.clear();
    ss.seekg(0);

    // read the ways
    std::vector<int> vNodeID;
    bool fway = false;
    bool fnodes = false;
    while (getline(ss, line))
    {
        if (!fway)
        {
            // outside a way

            int pdbg = line.find("\"nodes\":");
            if ((int)line.find("\"nodes\":") > 0)
            {
                fway = true;
                fnodes = true;
            }
        }
        else
        {
            // inside a way

            if (fnodes)
            {
                if ((int)line.find("],") > 0)
                {
                    // all nodes read
                    fnodes = false;
                }
                else
                {
                    // add another node
                    vNodeID.push_back(
                        atoi(line.c_str()));
                }
            }
            else
            {
                int pname = line.find("\"name\"");
                if (pname > 0)
                {
                    auto sName = line.substr(pname + 9);
                    sName = sName.substr(0, sName.length() - 2);
                    myVWay.emplace_back(sName, vNodeID);
                    vNodeID.clear();
                    fway = false;
                }
            }
        }
    }
}

void cOSM::calculateBBox()
{
    /*
        lattitude
            runs from south to north
            +ve in northern hemispehere

            north edge of box is greatest lat value
            south edge of box is least lat value

            becomes y pixel location
            but must be multiplied by a -ve
            because y pixel value runs from north to south

        longitude
            runs from west to east
            -ve in North America

            west edge of box is least ( most -ve ) value
            east edge of box is greatest value

            becomes x pixel location


    */
    double north, west, south, east;
    double lat, lon;
    north = -500;
    south = 500;
    west = 500;
    east = -500;
    for (auto &idn : myNode)
    {
        idn.second.getlatlon(lat, lon);
        if (lat > north)
            north = lat;
        if (lat < south)
            south = lat;
        if (lon < west)
            west = lon;
        if (lon > east)
            east = lon;
    }

    calcOffsetScale(
        north, west, south, east);

    cxy pnw = latlon2pixel(north, west);
    std::cout << "North West " << pnw.x << " " << pnw.y << "\n";
    // std::cout <<"South East ";
    // latlon2pixel(south,east);
    std::cout << north << " " << west << " " << south << " " << east << "\n";
    std::cout << "OSM bbox S:" << south << " W:" << west
              << " N:" << north << " E:" << east << "\n";
}

void cOSM::calculatePixel()
{
    double lat, lon;
    for (auto &idn : myNode)
    {
        idn.second.getlatlon(lat, lon);
        idn.second.setPixel(
            latlon2pixel(lat, lon));
    }
}

double cWayOSM::distance2(
    const cxy &point,
    const cOSM &theOSM,
    cxy &closest)
{
    double bestDist = INT_MAX;
    cxy prev(-1, -1);
    for (const auto &n : myVNodeID)
    {
        // get node pixel location
        cxy px = theOSM.getNode(n).getPixel();
        if (prev.x < 0)
        {
            prev = px;
            continue;
        }

        double d = point.dis2toline(prev, px);
        if (d < bestDist)
        {
            bestDist = d;
            closest = point.closest(prev, px);
        }
    }
    return bestDist;
}

cxy cOSM::closestOnWay(const cxy &point)
{
    cxy closest;
    double bestDist = INT_MAX;
    for (auto &w : myVWay)
    {
        cxy cp;
        double d = w.distance2(point, *this, cp);
        if (d < bestDist)
        {
            bestDist = d;
            closest = cp;
        }
    }
    return closest;
}

const cNodeOSM &
cOSM::getNode(const int id) const
{
    try
    {
        return myNode.at(id);
    }
    catch (...)
    {
        throw std::runtime_error(
            "Missing OSM node");
    }
}
double
cOSM::Haversine(
    std::pair<double, double> from,
    std::pair<double, double> to)
{
    // http://blog.julien.cayzac.name/2008/10/arc-and-distance-between-two-points-on.html

    const double DEG_TO_RAD = 0.017453292519943295769236907684886;
    const double EARTH_RADIUS_IN_METERS = 6372797.560856;

    double latitudeArc = (from.first - to.first) * DEG_TO_RAD;
    double longitudeArc = (from.second - to.second) * DEG_TO_RAD;
    double latitudeH = sin(latitudeArc * 0.5);
    latitudeH *= latitudeH;
    double lontitudeH = sin(longitudeArc * 0.5);
    lontitudeH *= lontitudeH;
    double tmp = cos(from.first * DEG_TO_RAD) * cos(to.first * DEG_TO_RAD);
    return EARTH_RADIUS_IN_METERS * 2.0 * asin(sqrt(latitudeH + tmp * lontitudeH));
}

void cOSM::calcOffsetScale(
    double north, double west, double south, double east)
{
    const int displayXPixelCount = 1000;
    const int displayYPixelCount = 1000;

    // set north west corner of box
    // gets zero x,y pixel location

    mylatOff = -north;
    mylonOff = -west;

    double osmbbxHeightMeters = Haversine(
        std::make_pair(north, west),
        std::make_pair(south, west));
    double osmbbxWidthMeters = Haversine(
        std::make_pair(north, west),
        std::make_pair(north, east));
    double sqbxMeters = osmbbxHeightMeters;
    if (sqbxMeters > osmbbxWidthMeters)
        sqbxMeters = osmbbxWidthMeters;
    double metersPerLat = osmbbxHeightMeters / (north - south);
    double metersPerLon = osmbbxWidthMeters / (east - west);
    double minlat = north - sqbxMeters / metersPerLat;
    double maxlat = north;
    double minlon = west;
    double maxlon = west + sqbxMeters / metersPerLon;

    mylonScale = displayXPixelCount / (maxlon - minlon);
    mylatScale = -displayYPixelCount / (maxlat - minlat);
}

void cOSM::download()
{

// https://github.com/yhirose/cpp-httplib

    httplib::Client cli("http://overpass-api.de");

    std::string query = 
        std::string("/api/interpreter/?data=<osm-script output=\"json\">")
        + "<query type=\"way\">" 
        + makeOverpassBBox()
        + "<has-kv k=\"highway\" regv=\"primary|secondary|tertiary|residential\"/>"
        + "</query>" 
        + "<union><item />"
        + "<recurse type=\"way-node\"/>"
        + "</union><print/></osm-script>";
    std::cout << "Query: " << query << "\n";

    auto res = cli.Get(query);

    std::cout << "status " << res->status << "\n"
              << "downloaded " << res->body.length() << std::endl;

    std::ofstream ofs("../dat/osm_download.txt");
    if( ! ofs.is_open() )
        throw std::runtime_error(
            "osm download file not open"        );
    ofs << res->body;

    std::istringstream ss(res->body.c_str());
    parse( ss );


}

void cOSM::setBBox(
    double NorthLat,
    double WestLon,
    double SouthLat,
    double EastLon)
{
    myNorthLat = NorthLat;
    myWestLon = WestLon;
    mySouthLat = SouthLat;
    myEastLon = EastLon;
}
    void cOSM::getBBox(
        double &NorthLat,
        double &WestLon,
        double &SouthLat,
        double &EastLon) const
        {
               NorthLat = myNorthLat;
    WestLon = myWestLon;
    SouthLat = mySouthLat;
    EastLon = myEastLon; 
        }

std::string cOSM::makeOverpassBBox()
{
    std::string q = "<bbox-query s=\"" + std::to_string(mySouthLat) + "\" w=\"" + std::to_string(myWestLon) + "\" n=\"" + std::to_string(myNorthLat) + "\" e=\"" + std::to_string(myEastLon) + "\"/>";
    std::cout << q << "\n";
    return q;
}