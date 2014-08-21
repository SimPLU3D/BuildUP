#ifndef ENERGY_END_TEST_PLU
#define ENERGY_END_TEST_PLU

namespace simulated_annealing
{
    class  energy_end_test {
    public:
        energy_end_test(double eBorder,double eFAR,double eTotal) : m_eBorder(eBorder),m_eFAR(eFAR),m_eTotal(eTotal) {}
        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration& c, const Sampler& s, double) {
            return (c.energy_dBorder()<=m_eBorder && c.energy_far()<=m_eFAR && c.energy()<=m_eTotal);

        }
    private:
        double m_eBorder;
        double m_eFAR;
        double m_eTotal;
    };
};

#endif // ENERGY_END_TEST_PLU
