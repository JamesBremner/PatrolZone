/// @brief Header only C++ wrapper for Open Street Map Overpass API
//
// https://wiki.openstreetmap.org/wiki/Overpass_API/Language_Guide
//
// Dependency:  cpp-httplib  https://github.com/yhirose/cpp-httplib

// (c) 2023 James Bremner
//

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// if OSM host reuires SSL ( i.e. https://... )
// uncomment the following define
// but note unresolved documentation errors in cpp-httplib build instructions
// these libraries will have to be added to the link in this order
// -lcrypt32 -lssl -lcrypto -lws2_32
// details and any updates https://github.com/yhirose/cpp-httplib/issues/1669

// #define CPPHTTPLIB_OPENSSL_SUPPORT

#include "httplib.h"

class cOSMOverpass
{
public:
    /// @brief CTOR sets some default parameters
    cOSMOverpass()
        : myNorthLat(501), myWestLon(501), mySouthLat(501), myEastLon(501),
          myHost("http://overpass-api.de")
    {
    }

    /// @brief set URL for OSM overpass host
    /// @param hostURL 
    ///
    /// If this is not called the default host will be used
    /// http://overpass-api.de

    void setHost( const std::string& hostURL )
    {
        myHost = hostURL;
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

        If this is not called the bounding box will be undefined
        and the doQuery method will throw an exception
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
    void doQuery()
    {

        // https://github.com/yhirose/cpp-httplib

        httplib::Client cli(myHost);

        makeQuery();

        auto res = cli.Get(myQuery);

        myStatus = res->status;

        myDownload = res->body;
    }

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
    const std::string &getDownload() const
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