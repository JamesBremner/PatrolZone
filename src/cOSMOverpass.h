/// @brief C++ wrapper for Open Street Map Overpass API
//
// https://wiki.openstreetmap.org/wiki/Overpass_API/Language_Guide
//
// Dependency:  cpp-httplib  https://github.com/yhirose/cpp-httplib

// (c) 2023 James Bremner
//

// #define WIN32_LEAN_AND_MEAN
// #include <Windows.h>
// // #define CPPHTTPLIB_OPENSSL_SUPPORT
// #include "httplib.h"

class cOSMOverpass
{
public:
    /// @brief CTOR sets some default parameters
    cOSMOverpass()
        : myNorthLat(501), myWestLon(501), mySouthLat(501), myEastLon(501),
          myHost("http://overpass-api.de")
    {
    }

    /** @brief Set/Get bounding box
    /// @param NorthLat
    /// @param WestLon
    /// @param SouthLat
    /// @param EastLon

        lattitude
            runs from south to north
            +ve in northern hemisphere

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
        double EastLon)
    {
        myNorthLat = NorthLat;
        myWestLon = WestLon;
        mySouthLat = SouthLat;
        myEastLon = EastLon;
    }

    /// @brief Execute Overpass query to get ways and nodes in bounding box
    void doQuery();
//     {

//     // https://github.com/yhirose/cpp-httplib

//     httplib::Client cli(myHost);

//     auto res = cli.Get(myQuery);

//     //std::cout << res->status << "\n";

//     myStatus = res->get()->status;

//     myDownload = res->body;
// }

    void getBBox(
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
    const std::string& getDownload() const
    {
        return myDownload;
    }

    int getStatus() const
    {
        return myStatus;
    }

    std::string getQuery() const
    {
        return myQuery;
    }

private:
    std::string myHost;

    // bounding box
    double myNorthLat, myWestLon, mySouthLat, myEastLon;

    std::string myQuery;

    // downloaded data
    std::string myDownload;

    int myStatus;

    std::string makeQueryBBox()
    {
        if (myNorthLat > 500)
            throw std::runtime_error(
                "cOSMOverpass Bounding box not set");
        std::string q = "<bbox-query s=\"" + std::to_string(mySouthLat) + "\" w=\"" + std::to_string(myWestLon) + "\" n=\"" + std::to_string(myNorthLat) + "\" e=\"" + std::to_string(myEastLon) + "\"/>";
        return q;
    }

    void makeQuery()
    {
        myQuery = 
        std::string("/api/interpreter/?data=<osm-script output=\"json\">") + "<query type=\"way\">" + makeQueryBBox() + "<has-kv k=\"highway\" regv=\"primary|secondary|tertiary|residential\"/>" + "</query>" + "<union><item /><recurse type=\"way-node\"/></union><print/></osm-script>";
    }
};