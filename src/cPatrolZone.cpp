#include <iostream>
#include <sstream>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <vector>
#include <cmath>

#include "cPatrolZone.h"

int cCluster::NextIndex = 0;

bool operator==(const cCluster &lhs, const cCluster &rhs)
{
    return (lhs.myIndex == rhs.myIndex);
}

void cPatrolZone::generateCrimeRandom(
    int number, // number to generate
    int minx,
    int maxx,
    int miny,
    int maxy)
{
    myVCrime.clear();
    srand(time(NULL));
    for (int k = 0; k < number; k++)
    {
        myVCrime.push_back(cCrime(rand() % maxx + minx, rand() % maxy + miny));
    }
}

void cPatrolZone::generateRoad()
{
    myVRoad.clear();
    for( int k = 0; k < 10; k++ )
    {
        myVRoad.push_back(
            cRoad(cxy(k*10,0),cxy(k*10,100),cRoad::eHighway::tertiary));
        myVRoad.push_back(
            cRoad(cxy(0,k*10),cxy(100,k*10),cRoad::eHighway::tertiary));
        
    }
}

void cPatrolZone::AHC(int number)
{
    myVCluster.clear();

    // Construct clusters, one for each crime
    for (auto &c : myVCrime)
    {
        std::vector<cCrime> cluster{c};
        myVCluster.push_back(cCluster(c));
    }
    std::pair<cCluster *, cCluster *> closest;

    // combine clusters until there are 10 left
    for (int k = 0; k < myVCluster.size() - number; k++)
    {
        float dmin = 1e10;
        for (auto &cluster1 : myVCluster)
        {
            for (auto &cluster2 : myVCluster)
            {
                if (cluster1 == cluster2)
                    continue;
                if (cluster1.myIndex == -1)
                    continue;
                if (cluster2.myIndex == -1)
                    continue;
                float d = cluster1.Distance(cluster2);
                if (d < dmin)
                {
                    dmin = d;
                    closest.first = &cluster1;
                    closest.second = &cluster2;
                }
            }
        }
        // std::cout << "closest " << closest.first->Text() <<" and " << closest.second->Text() << "\n";
        closest.first->Combine(*closest.second);
    }
}

    void cPatrolZone::move2Road()
    {
        for( auto& c : myVCluster )
        {
            int x,y;
             c.getLocation(
                x,
                y            );
            cxy cluster_location(x,y);

            // find nearest road
            double bestDistance = INT_MAX;
            cRoad bestRoad;
            for( auto& r : myVRoad )
            {
                auto d = cluster_location.dis2toline(
                    r.myVNode[0],
                    r.myVNode.back() );
                if( d < bestDistance ) {
                    bestDistance = d;
                    bestRoad = r;
                }
            }
            // center the patrol on the road
            c.setRoadLocation( cluster_location.closest(
                 bestRoad.myVNode[0],
                 bestRoad.myVNode.back() ) );

        }
    }

float cCrime::Distance(const cCrime &other)
{
    return sqrt((myX - other.myX) * (myX - other.myX) + (myY - other.myY) * (myY - other.myY));
}

void cCluster::getLocation(int &x, int &y) const
{
    x = 0;
    y = 0;
    for( auto& c : myCrime )
    {
        int cx, cy;
        c.getLocation(cx,cy);
        x += cx;
        y += cy;
    }
    x /= myCrime.size();
    y /= myCrime.size();
}

void cPatrolZone::textDisplay()
{
    std::cout << "Crime Locations:\n";
    for (auto &c : myVCrime)
    {
        std::cout << c.Text() << ",";
    }
    std::cout << "\n";

    std::cout << "Clusters:\n";
    for (auto &cluster : myVCluster)
    {
        if (cluster.myIndex != -1)
            std::cout << cluster.Text() << "\n";
    }
}
