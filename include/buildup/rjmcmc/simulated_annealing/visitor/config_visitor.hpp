#ifndef CONFIG_VISITOR_HPP_INCLUDED
#define CONFIG_VISITOR_HPP_INCLUDED

namespace simulated_annealing
{

template<typename Configuration>
class config_visitor
{
public:
    config_visitor(unsigned int ibegin,unsigned int nbsave,Configuration* c)
        :m_begin(ibegin),m_save(nbsave),m_iter(0),m_config(c) {}

    void init(int,int s)
    {
//            m_save = s;
//            m_iter = 0;
    }

    template<typename Sampler>
    void begin(const Configuration& config, const Sampler&, double)
    {
        m_iter = 0;
        save(config);
    }

    template<typename Sampler>
    void end(const Configuration& c, const Sampler&, double)
    {
        save(c);
        std::cout<<"\niteration "<<m_iter<<"\n";
        //check border rule
        std::cout<<"final size "<<c.size()<<"energy "<<c.energy()<<" "<<c.energy_dBorder()<<" "<<c.energy_dPair()<<" "<<c.energy_hDiff()<<" "<<c.energy_lcr()<<" "<<c.energy_far()<<"\n";
        std::cout<<"ref   size "<<m_config->size()<<"energy "<<m_config->energy()<<" "<<m_config->energy_dBorder()<<" "<<m_config->energy_dPair()<<" "<<m_config->energy_hDiff()<<" "<<m_config->energy_lcr()<<" "<<m_config->energy_far()<<"\n";

    }
    template<typename Sampler>
    void visit(const Configuration& config, const Sampler&, double)
    {
        ++m_iter;
        if(m_iter>=m_begin && m_iter%m_save==0)
        {
            save(config);
        }
    }

private:
    unsigned int m_begin,m_save,m_iter;
    Configuration* m_config;

    void save(const Configuration& config)
    {
//            std::cout<<"\niter "<<m_iter<<"\n";
//            std::cout<<"energy stored "<<m_config->energy()<<"\n";
//            std::cout<<"energy cur "<<config.energy()<<"\n";
        if(config.energy_far()< (*m_config).energy_far())
            (*m_config) = config;
    }
};

}// namespace simulated_annealing

#endif // CONFIG_VISITOR_HPP_INCLUDED
