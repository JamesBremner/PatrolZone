#include "cPatrolZone.h"
#include "cOSM.h"

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
        double& latOff, double& latScale,
        double& lonOff, double& lonScale) const
        {
            latOff = mylatOff;
            latScale = mylatScale;
            lonOff = mylonOff;
            lonScale = mylonScale;
        }

bool cOSM::pixelSanity(const cxy& p )
{
    return ! ( p.x < 0 || p.y < 0 || p.x > 1200 || p.y > 1200);
}

cxy cOSM::latlon2pixel(double lat, double lon)
{
    cxy p(
        (int)(mylonScale * (mylonOff + lon)),
        (int)(mylatScale * (mylatOff + lat)));
    if (!pixelSanity( p ))
    {
        std::cout 
            <<"Offsets "<<mylatOff<<" "<<mylonOff
            <<"\n"<< lat << "," << lon 
            << " -> " << mylonOff + lon <<","<<mylatOff + lat
            << " -> " << p.x << "," << p.y 
            << "\n";
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

    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "cOSM cannot open file");

    int id;
    double lat, lon;
    std::string line;

    double latmin,latmax, lonmin,lonmax;
    int xmin,xmax, ymin,ymax;
    latmin = 500;
    lonmin = 500;
    latmax  = -500;
    lonmax = -500;

    while (getline(ifs, line))
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
                        id, lat, lon )));

            }
        }
    }


    calculateBBox();
    calculatePixel();

    // for( auto& idn : myNode )
    // {
    //     double lat,lon;
    //     idn.second.getlatlon(lat,lon);
    //     idn.second.setPixel( latlon2pixel(lat,lon));
    // }

    // std::cout << latmin <<","<< lonmin << "  " 
    //     <<latmax<<","<<lonmax<<"\n";

    // rewind
    ifs.clear();
    ifs.seekg(0);

    // read the ways
    std::vector<int> vNodeID;
    bool fway = false;
    bool fnodes = false;
    while (getline(ifs, line))
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
        south edge og box is least lat value

        becomes y pixel location 
        but mist be multiplied by a -ve
        becuase y pixel value runs from north to south

    longitude
        runs from west to east
        -ve in North America

        west edge of box is least ( most -ve ) value
        east edge of box is greatest value 

        becomes x pixel location


*/
    double north, west, south, east;
    double lat,lon;
    north = -500;
    south = 500;
    west = 500;
    east = -500;
    for( auto& idn : myNode )
    {
        idn.second.getlatlon( lat, lon );
        if( lat > north )
            north = lat;
        if( lat < south)
            south = lat;
        if( lon < west )
            west = lon;
        if( lon > east )
            east = lon;
    }

    // set north west corner of box
    // gets zero x,y pixel location

    mylatOff = - north;
    mylonOff = - west;

    // std::cout <<"North West ";
    // latlon2pixel(north,west);
    // std::cout <<"South East ";
    // latlon2pixel(south,east);
    // std::cout << north <<" "<< west <<" "<< south <<" "<< east <<"\n";

}

void cOSM::calculatePixel()
{
    double lat,lon;
    for( auto& idn : myNode )
    {
        idn.second.getlatlon( lat, lon );
        idn.second.setPixel(
            latlon2pixel(lat,lon)        );
    }
}