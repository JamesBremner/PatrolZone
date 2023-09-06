class cOSM
{
public:
    void set(
        double latOff, double latScale,
        double lonOff, double lonScale);

    void read(const std::string &fname);

    const std::vector<cxy>& 
    getNodes() const
    {
        return myVNode;
    }

private:
    std::vector<cxy> myVNode;
    double mylatOff, mylonOff;
    double mylatScale, mylonScale;
};