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

void cPatrolZone::GenerateCrimeRandom(
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
