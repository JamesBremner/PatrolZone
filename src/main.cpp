#include <iostream>
#include <sstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include <cmath>

using namespace std;

class cCrime
{
public:

    cCrime( float x, float y )
        : myX( x )
        , myY( y )
    {

    }
    /** Distance form this crime to another crime

    In a large city this should be calculated from  the Haversine formula
    but for now we will just use Pythagorus

    */
    float Distance( const cCrime& other )
    {
        return sqrt( (myX-other.myX)*(myX-other.myX) + (myY-other.myY)*(myY-other.myY));
    }
    std::string Text()
    {
        std::stringstream ss;
        ss << myX <<" "<< myY;
        return ss.str();
    }
private:
    float myX;
    float myY;
};

/** A cluster of nearbye crimes */
class cCluster
{
public:

    cCluster()
    {
        myIndex = ++NextIndex;
    }
    cCluster( const cCrime& c )
        : cCluster()
    {
        myCrime.push_back( c );
    }
    /** Distance between clusters, measured as greatest distance between two crimes in each cluster */
    float Distance( const cCluster& other )
    {
        float maxlinklength = -1;
        for( auto& c : myCrime )
        {
            for( auto& o : other.myCrime )
            {
                float d = c.Distance( o );
                if( d > maxlinklength )
                    maxlinklength = d;
            }
        }
        return maxlinklength;
    }

    void Combine( cCluster& other )
    {
        for( auto& c : other )
        {
            myCrime.push_back( c );
        }
        other.Delete();
    }

    void Delete()
    {
        myIndex = -1;
    }

    std::vector< cCrime >::iterator begin()
    {
        return myCrime.begin();
    }
    std::vector< cCrime >::iterator end()
    {
        return myCrime.end();
    }

    std::string Text()
    {
        std::stringstream ss;
        if( myIndex != -1 )
        {

            ss << "( "<< myIndex <<": ";
            for( auto& c : myCrime )
            {
                ss << c.Text() << ", ";
            }
            ss << " )";
        }
        return ss.str();
    }

    int myIndex;
private:

    static int NextIndex;
    std::vector< cCrime > myCrime;
};

int cCluster::NextIndex = 0;

operator==(const cCluster& lhs, const cCluster& rhs)
{
    return ( lhs.myIndex == rhs.myIndex );
}

/** Generate Random Crimes for simulation
    @param[in] number to generate
    @param[in] minx ... maxy  extent of locations
    @return vector of generated crimes
*/
std::vector< cCrime >
GenerateCrimeRandom(
    int number,     // number to generate
    int minx,
    int maxx,
    int miny,
    int maxy
)
{
    std::vector< cCrime > vc;
    srand (time(NULL));
    for( int k = 0; k < number; k++ )
    {
        vc.push_back( cCrime( rand() % maxx + minx,  rand() % maxy + miny ) );
    }
    return vc;
}

/** agglomerative hierarchical clustering
    @param[in] vc vector of crimes
    @param[in] number of clusters required
    @return clusters of crimes
*/
std::vector< cCluster > HAC( const std::vector< cCrime >& vc, int number )
{
    // Construct clusters, one for each crime
    std::vector< cCluster > vClusters;
    for( auto& c : vc )
    {
        std::vector< cCrime > cluster { c };
        vClusters.push_back( cCluster( c ) );
    }
    std::pair< cCluster*, cCluster* > closest;

    // combine clusters until there are 10 left
    for( int k = 0; k < vClusters.size() - number; k++ )
    {
        float dmin = 1e10;
        for( auto& cluster1 : vClusters )
        {
            for( auto& cluster2 : vClusters )
            {
                if( cluster1 == cluster2 )
                    continue;
                if( cluster1.myIndex == -1 )
                    continue;
                if( cluster2.myIndex == -1 )
                    continue;
                float d = cluster1.Distance( cluster2 );
                if( d < dmin )
                {
                    dmin = d;
                    closest.first = &cluster1;
                    closest.second = &cluster2;
                }
            }
        }
        //std::cout << "closest " << closest.first->Text() <<" and " << closest.second->Text() << "\n";
        closest.first->Combine( *closest.second );
    }

    return vClusters;
}

int main()
{
    std::vector< cCrime > vc = GenerateCrimeRandom( 100, 0, 100, 0, 100 );

    std::cout << "Crime Locations:\n";
    for( auto& c : vc )
    {
        std::cout << c.Text() << ",";
    }
    std::cout << "\n";

    std::vector< cCluster > Clusters = HAC( vc, 10 );

    std::cout << "Clusters:\n";
    for( auto& cluster : Clusters )
    {
        if( cluster.myIndex != -1 )
            std::cout << cluster.Text() << "\n";
    }

    return 0;
}
