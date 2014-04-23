#ifndef ENERGY_VISITOR_HPP_INCLUDED
#define ENERGY_VISITOR_HPP_INCLUDED

namespace simulated_annealing {

    class energy_visitor
    {
    public:
        energy_visitor(std::ofstream& ofs) : m_iter(0), m_out(ofs) {}


        void init(int, int s)
        {
            m_save = s;
            m_iter = 0;
        }

        template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler&, double)
        {
            m_iter = 0;
            m_out<<m_iter<<" "<<config.energy()<<" eBorder ePair  eCES  eCOS\n";
            save(config);
        }

        template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler&, double)
        {
            save(config);
            //std::cout<<m_iter<<" "<<config.energy()<<" "<<config.energy_dBorder()<<" "<<config.energy_dPair()<<" "<<config.energy_ces()<<" "<<config.energy_cos()<<"\n";
        }
        template<typename Configuration, typename Sampler>
        void visit(const Configuration& config, const Sampler&, double){
            if((++m_iter)%m_save==0)
            {
                save(config);
                //std::cout<<"iteration:"<<m_iter<<" eU "<<config.unary_energy()<<" eB "<<config.binary_energy()<<" eG "<<config.global_energy()<<"\n";
            }
        }

    private:
        unsigned int m_save,m_iter;
        std::ofstream& m_out;

        template<typename Configuration>
        void save(const Configuration& c) const
        {
            m_out<<m_iter<<" "<<c.energy()<<" "<<c.energy_dBorder()<<" "<<c.energy_dPair()<<" "<<c.energy_ces()<<" "<<c.energy_cos()<<"\n";
           // m_out<<m_iter<<" "<<c.audit_energy()<<" "<<c.audit_dBorder()<<" "<<c.audit_dPair()<<" "<<c.audit_ces()<<" "<<c.audit_cos()<<"\n";
//            if(c.energy_dPair()!=0 || c.audit_dPair()!=0)
//            {
//            std::cout<<m_iter<<" energy "<<c.energy()<<" eBorder "<<c.energy_dBorder()<<" ePair "<<c.energy_dPair()<<" eCes "<<c.energy_ces()<<" eCos "<<c.energy_cos()<<"\n";
//            std::cout<<m_iter<<" audit  "<<c.audit_energy()<<" eBorder "<<c.audit_dBorder()<<" ePair "<<c.audit_dPair()<<" eCes "<<c.audit_ces()<<" eCos "<<c.audit_cos()<<"\n";
//            }
        }
    };

}// namespace simulated_annealing

#endif // ENERGY_VISITOR_HPP_INCLUDED
