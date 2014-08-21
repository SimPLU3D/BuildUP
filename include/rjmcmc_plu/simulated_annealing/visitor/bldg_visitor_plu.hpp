#ifndef BLDG_VISITOR_HPP
#define BLDG_VISITOR_HPP

#include <fstream>

namespace simulated_annealing {

    class bldg_visitor
    {
    public:
        bldg_visitor(std::ofstream& ofs) : m_iter(0), m_out(ofs) {}


        void init(int,int s)
        {
            m_save = s;
            m_iter = 0;
        }

        template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler&, double)
        {
            m_iter = 0;
            save(config);
        }

        template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler&, double)
        {
            save(config);
        }
        template<typename Configuration, typename Sampler>
        void visit(const Configuration& config, const Sampler&, double){
            if((++m_iter)%m_save==0)
            {
                save(config);
            }
        }

    private:
        unsigned int m_save,m_iter;
        std::ofstream& m_out;

        template<typename Configuration>
        void save(const Configuration& config) const
        {
            //    std::cout<<"iter "<< m_iter<<"\n";
            typename Configuration::const_iterator it = config.begin(), end = config.end();
            for (; it != end; it++)
            {
//                //test check width
//                std::cout<<"rho "<<config[it].rho<<"\n";
//                std::cout<<"theta "<<config[it].theta<<"\n\n";
//                std::cout<<"h "<<config[it].h<<"\n\n";

                m_out<<m_iter<<" ";

                for(int i=0;i<5;i++)
                {
                    double x = geometry::to_double(config[it].bottom().point(i).x());
                    double y = geometry::to_double(config[it].bottom().point(i).y());
                    //std::cout<<x <<","<<y<<" ";
                    m_out << x <<" "<<y<<" ";
                }
               // std::cout<<std::endl;
                m_out<<geometry::to_double(config[it].h())<<"\n";
            }
       }
    };

}// namespace simulated_annealing

#endif // BLDG_VISITOR_HPP

