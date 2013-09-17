/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef TEX_VISITOR_HPP
#define TEX_VISITOR_HPP

#include <string>
#include <fstream>
#include "Lot.hpp"

#ifdef  GEOMETRY_RECTANGLE_2_HPP
template<typename K>
void tex_write_object(std::ostream& out, const geometry::Rectangle_2<K>& r)
{
    // TODO
}
#endif //  GEOMETRY_RECTANGLE_2_HPP

#ifdef  GEOMETRY_CIRCLE_2_HPP
template<typename K>
void tex_write_object(std::ostream& out, const geometry::Circle_2<K>& c)
{
    typename geometry::Circle_2<K>::Point_2 p = c.center();
    double x0 = geometry::to_double(p.x());
    double y0 = geometry::to_double(p.y());
    double r  = geometry::to_double(c.radius());
    out << "\\draw (" << x0 << "," << y0 << ")" << " circle ("<<r<<");\n";
}
#endif // GEOMETRY_CIRCLE_2_HPP

template<typename K>
void tex_write_object(std::ostream& out, const geometry::Cuboid_bldg<K>& c)
{
    geometry::Rectangle_2<geometry::Simple_cartesian<double>> a = c.rect_2;
    for(int i=0;i<5;i++)
    {
        double x = CGAL::to_double(a.point(i).x());
        double y = CGAL::to_double(a.point(i).y());
        //std::cout<<x <<","<<y<<" ";
        out << x <<" "<<y<<" ";
    }
   // std::cout<<std::endl;
    out<<c.h<<"\n";
}

struct tex_writer
{

public:
    tex_writer(const std::string& file) {
        m_file.open(file.c_str());
        //m_file << "\\begin{tikzpicture}\n";
    }
    ~tex_writer() {
        //_file << "\\end{tikzpicture}\n";
    }

    typedef void result_type;

    inline operator bool() { return m_file; }

    template<typename T> void operator()(const T& t) const {
        tex_write_object(m_file,t);
    }

private:
    mutable std::ofstream m_file;
};

#include <iostream>
#include <iomanip>
#include <sstream>
#include "rjmcmc/variant.hpp"

namespace simulated_annealing {

    class tex_visitor
    {
    public:
        tex_visitor(const std::string& prefix, Lot* lot)
        : m_iter(0), m_prefix(prefix), m_lot(lot) {}

        void init(int, int s)
        {
            m_save = s;
            m_iter = 0;
            //m_file_energy.open("energy.tex");
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
        void visit(const Configuration& config, const Sampler&, double) {
            if((++m_iter)%m_save==0)
                save(config);
        }
    private:
        unsigned int m_save, m_iter;
        std::string m_prefix;
        Lot* m_lot;

        template<typename Configuration>
        void save(const Configuration& config) const
        {
            std::ostringstream oss;
            oss << m_prefix << std::setw(15) << std::setfill('0') << m_iter << ".tex";
            tex_writer writer(oss.str());
            if(!writer)
            {
                std::cout << "\tUnable to create tex file " << oss.str() << std::endl;
                return;
            }


            typename Configuration::const_iterator it = config.begin(), end = config.end();
            double total_area = 0;
            double total_floor_area = 0;
            double hMin = config[it].h, hMax = config[it].h;
            double areaMin = config[it].area(), areaMax =areaMin;
            double distUrMin = config[it].distance2line(m_lot->_mainEdge.source(),m_lot->_mainEdge.target()),distUrMax = distUrMin;
            typename Configuration::const_iterator it2=it;
            double sqDistBiMin = config[it].squared_distance_min(config[++it2]), sqDistBiMax = sqDistBiMin;


            for (; it != end; ++it)
            {
                rjmcmc::apply_visitor(writer,config[it]);

                total_area += config[it].area();
                int nfloor = config[it].h/ m_lot->_rule._hMin;
                total_floor_area += config[it].area()*nfloor;

                double h=config[it].h;
                hMin = h < hMin? h:hMin;
                hMax = h > hMax? h:hMax;

                double area=config[it].area();
                areaMin = area<areaMin? area:areaMin;
                areaMax = area>areaMax? area:areaMax;

                double distUr=config[it].distance2line(m_lot->_mainEdge.source(),m_lot->_mainEdge.target());
                distUrMin = distUr < distUrMin? distUr:distUrMin;
                distUrMax = distUr > distUrMax? distUr:distUrMax;

                it2 = it;
                ++it2;

                for(;it2 != end; ++it2)
                {
                    double sqDistBi = config[it].squared_distance_min(config[it2]);
                    sqDistBiMin = sqDistBi < sqDistBiMin? sqDistBi:sqDistBiMin;
                    sqDistBiMax = sqDistBi > sqDistBiMax? sqDistBi:sqDistBiMax;
                }


            }
            std::cout<<"ces "<<total_area/m_lot->_area<<std::endl;
            std::cout<<"cos "<<total_floor_area/m_lot->_area<<std::endl;
            std::cout<<"hMin "<<hMin<<std::endl;
            std::cout<<"hMax "<<hMax<<std::endl;
            std::cout<<"areaMin "<<areaMin<<std::endl;
            std::cout<<"areaMax "<<areaMax<<std::endl;
            std::cout<<"distUrMin "<<distUrMin<<std::endl;
            std::cout<<"distUrMax "<<distUrMax<<std::endl;
            std::cout<<"distBiMin "<<sqrt(sqDistBiMin)<<std::endl;
            std::cout<<"distBiMax "<<sqrt(sqDistBiMax)<<std::endl;
        }
    };

} // namespace simulated_annealing

#endif // TEX_VISITOR_HPP

