class cCrime
{
public:
    cCrime(float x, float y)
        : myX(x), myY(y)
    {
    }
    /** Distance form this crime to another crime

    In a large city this should be calculated from  the Haversine formula
    but for now we will just use Pythagorus

    */
    float Distance(const cCrime &other);

    std::string Text() const
    {
        std::stringstream ss;
        ss << myX << " " << myY;
        return ss.str();
    }

    void getLocation(int& x, int& y ) const
        {
            x = myX;
            y = myY;
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
    cCluster(const cCrime &c)
        : cCluster()
    {
        myCrime.push_back(c);
    }
    /** Distance between clusters, measured as greatest distance between two crimes in each cluster */
    float Distance(const cCluster &other)
    {
        float maxlinklength = -1;
        for (auto &c : myCrime)
        {
            for (auto &o : other.myCrime)
            {
                float d = c.Distance(o);
                if (d > maxlinklength)
                    maxlinklength = d;
            }
        }
        return maxlinklength;
    }

    void getLocation( int& x, int& y ) const;

    void Combine(cCluster &other)
    {
        for (auto &c : other)
        {
            myCrime.push_back(c);
        }
        other.Delete();
    }

    void Delete()
    {
        myIndex = -1;
    }
    bool isDeleted() const
    {
        return ( myIndex < 0 );
    }

    std::vector<cCrime>::iterator begin()
    {
        return myCrime.begin();
    }
    std::vector<cCrime>::iterator end()
    {
        return myCrime.end();
    }

    std::string Text()
    {
        std::stringstream ss;
        if (myIndex != -1)
        {

            ss << "( " << myIndex << ": ";
            for (auto &c : myCrime)
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
    std::vector<cCrime> myCrime;
};

class cPatrolZone
{
public:
    /** Generate Random Crimes for simulation
        @param[in] number to generate
        @param[in] minx ... maxy  extent of locations
    */
    void
    GenerateCrimeRandom(
        int number, // number to generate
        int minx,
        int maxx,
        int miny,
        int maxy);

    /** agglomerative hierarchical clustering
        @param[in] number of clusters required
    */
    void
    AHC(int number);

    void textDisplay();

    const std::vector<cCrime>& getCrime() const
    {
        return myVCrime;
    }
    const std::vector<cCluster>& getCluster() const
    {
        return myVCluster;
    }

private:
    std::vector<cCrime> myVCrime;
    std::vector<cCluster> myVCluster;
};
