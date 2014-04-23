#ifndef IO_BLDG_HPP
#define IO_BLDG_HPP

#include "sfcgal.hpp"
#include "plu/Lot.hpp"
#include "plu/Bldg.hpp"
#include <fstream>
//#include <gdal/ogrsf_frmts.h>
namespace io
{


    template<typename Configuration>
    void save2bldg(const Configuration& config,Lot* lot,std::vector<Bldg>& bldgs)
    {
        typename Configuration::const_iterator it = config.begin(), end = config.end();
        for (; it != end; ++it)
        {
            std::vector<Point> points;
            for(int i=0;i<5;i++)
            {
                double x = CGAL::to_double(config[it].rect_2.point(i).x());
                double y = CGAL::to_double(config[it].rect_2.point(i).y());
                points.push_back(Point(x,y,0));

            }
            LineString ring(points);
            Polygon footprint(ring);
            bldgs.push_back(Bldg(footprint,config[it].h,lot));
        }

    }

    void loadEvolution(const char* txt,std::map< int,std::vector<Bldg> >& iter_bldgs)
    {
        std::ifstream in(txt);
        if(in.is_open())
        {
            int iter;
            double x,y,h;
            in>>iter;
            while(!in.eof())
            {
                std::vector<Point> points;
                for(int i=0;i<5;++i)
                {
                    in>>x;
                    in>>y;
                    points.push_back(Point(x,y,0));
                }
                in>>h;
                LineString ring(points);
                Polygon footprint(ring);

                iter_bldgs[iter].push_back(Bldg(footprint,h));

                in>>iter;
            }
        }
    }
}//namespace io

#endif // IO_BLDG_HPP
