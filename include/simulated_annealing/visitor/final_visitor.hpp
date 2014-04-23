#ifndef FINAL_VISITOR_HPP
#define FINAL_VISITOR_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include "rjmcmc/variant.hpp"

namespace simulated_annealing {

    //operations only at the end
    class final_visitor
    {
    public:
        final_visitor(Lot* lot): m_iter(0), m_lot(lot) {}

        void init(int, int )
        {
            //m_save = s;
            m_iter = 0;
            //m_file_energy.open("energy.tex");
        }

        template<typename Configuration, typename Sampler>
        void begin(const Configuration& config, const Sampler&, double)
        {
            m_iter = 0;
            //save(config);
        }

        template<typename Configuration, typename Sampler>
        void end(const Configuration& config, const Sampler&, double)
        {
            save(config);
            verify(config);
        }

        template<typename Configuration, typename Sampler>
        void visit(const Configuration& config, const Sampler&, double) {
            //if((++m_iter)%m_save==0)
            //    save(config);
        }

    private:
        unsigned int m_iter;
        Lot* m_lot;



        template<typename Configuration>
        void verify(const Configuration& config) const
        {
            typename Configuration::const_iterator it = config.begin(), end = config.end();
            double total_area = 0;
            double total_floor_area = 0;
            double hMin = config[it].h, hMax = config[it].h;
            double areaMin = config[it].area(), areaMax =areaMin;
            std::vector<double> distsUr;
            typename Configuration::const_iterator it2=it;
            double sqDistBiMin = config[it].squared_distance_min(config[++it2]), sqDistBiMax = sqDistBiMin;
            double overlap_area = 0;

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

                double distUr=config[it].distance2line(m_lot->_mainEdge);
                if(distUr>m_lot->_rule._distTolUr && distUr<m_lot->_rule._distMinUr)
                    distsUr.push_back(distUr);

                //check border h
                //if(distUr<=2)


                it2 = it;
                ++it2;

                for(;it2 != end; ++it2)
                {
                    overlap_area += geometry::intersection_area(config[it],config[it2]);
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
            if(distsUr.empty())
                std::cout<<"no unsatisfied distUr "<<std::endl;
            else
            {
                for(int i=0;i<distsUr.size();++i)
                    std::cout<<"unsatisfied distUr "<<distsUr[i]<<std::endl;
            }

            std::cout<<"distBiMin "<<sqrt(sqDistBiMin)<<std::endl;
            std::cout<<"distBiMax "<<sqrt(sqDistBiMax)<<std::endl;
            std::cout<<"overlap area "<<overlap_area<<std::endl;

        }
    };

} // namespace simulated_annealing

#endif // FINAL_VISITOR_HPP
