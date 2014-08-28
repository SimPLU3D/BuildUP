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
        return (c.energy_dBorder()<600*c.size()&& c.energy_dPair()<1&& c.energy_hDiff()<1&& c.energy_far()<1800);

    }
};
};

#endif // ENERGY_END_TEST_PLU
