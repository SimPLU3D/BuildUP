#ifndef GEOM_VISITOR_HPP
#define GEOM_VISITOR_HPP

#include <fstream>


namespace simulated_annealing
{

//output all intermedium configurations to the same ofstream
class geom_visitor
{
public:
    geom_visitor(unsigned int nbsave,std::ofstream& ofs,double transX=0,double transY=0)
        : m_save(nbsave),m_iter(0), m_out(ofs), m_transX(transX),m_transY(transY) {}


    void init(int,int s)
    {
//            m_save = s;
//            m_iter = 0;
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
    double m_transX;
    double m_transY;

    template<typename Configuration>
    void save(const Configuration& config) const
    {
        typename Configuration::const_iterator it = config.begin(), end = config.end();
        for (; it != end; it++)
        {
            m_out<<m_iter<<" ";

            for(int i=0; i<5; i++)
            {
                double x = config[it].bottom().point(i).x()+m_transX;
                double y = config[it].bottom().point(i).y()+m_transY;
                m_out << std::fixed<<x <<" "<<y<<" ";
            }
            m_out<<config[it].h()<<"\n";
        }
    }
};

}// namespace simulated_annealing

#endif // GEOM_VISITOR_HPP

