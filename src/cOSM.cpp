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

void cOSM::read(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "cOSM cannot open file");
    double lat, lon;
    std::string line;

    while (getline(ifs, line))
    {
        // std::cout << line << "\n";
        int p = line.find("\"lat\":");
        if (p >= 0)
        {
            lat = atof(line.substr(p + 6).c_str());
        }
        else
        {
            p = line.find("\"lon\":");
            if (p >= 0)
            {
                lon = atof(line.substr(p + 6).c_str());

                myVNode.push_back(cxy(
                    (int)mylatScale * (mylatOff + lat),
                    (int)mylonScale * (mylonOff + lon)));
            }
        }
    }
}