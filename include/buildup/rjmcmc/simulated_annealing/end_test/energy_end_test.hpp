#ifndef ENERGY_END_TEST_PLU
#define ENERGY_END_TEST_PLU

namespace simulated_annealing
{
class  energy_end_test
{
public:
    energy_end_test() {}
    template<typename Configuration, typename Sampler>
    inline bool operator()(const Configuration& c, const Sampler& s, double)
    {

//        return (c.energy_dBorder()<=600*c.size()
//                && c.energy_dPair()<=100
//                && c.energy_hDiff()<=100
//                && c.energy_lcr()<=200
//                && c.energy_far()<=730);


       return (c.energy_dBorder()<=300*c.size()
                && c.energy_dPair()<=100
                && c.energy_hDiff()<=100
                && c.energy_lcr()<=800
                && c.energy_far()<=3000);

    }

private:
    double m_eBorder;
    double m_eFAR;
    double m_eTotal;
};
};

#endif // ENERGY_END_TEST_PLU
