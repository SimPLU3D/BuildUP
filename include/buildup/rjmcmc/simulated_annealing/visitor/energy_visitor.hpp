#ifndef ENERGY_VISITOR_HPP
#define ENERGY_VISITOR_HPP

#include <fstream>
namespace simulated_annealing
{

class energy_visitor
{
public:
    inline energy_visitor(unsigned int nbsave,std::ofstream& ofs) : m_save(nbsave),m_iter(0), m_out(ofs) {}


    inline void init(int, int s)
    {
//            m_save = s;
//            m_iter = 0;
    }

    template<typename Configuration, typename Sampler>
    void begin(const Configuration& config, const Sampler&, double)
    {
        m_iter = 0;
        m_out<<m_iter<<" "<<config.energy()<<" eBorder  ePair  eHDiff eLCR  eFAR\n";
        save(config);
    }

    template<typename Configuration, typename Sampler>
    void end(const Configuration& config, const Sampler&, double)
    {
        save(config);
    }
    template<typename Configuration, typename Sampler>
    void visit(const Configuration& config, const Sampler&, double)
    {

        if((++m_iter)%m_save==0)
        {
            save(config);
        }
    }

private:
    unsigned int m_save,m_iter;
    std::ofstream& m_out;

    template<typename Configuration>
    void save(const Configuration& c) const
    {
        m_out<<m_iter<<" "<<c.energy()<<" "<<c.energy_dBorder()<<" "<<c.energy_dPair()<<" "<<c.energy_hDiff()<<" "<<c.energy_lcr()<<" "<<c.energy_far()<<"\n";
    }
};

}// namespace simulated_annealing

#endif // ENERGY_VISITOR_HPP
