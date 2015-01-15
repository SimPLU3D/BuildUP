#ifndef IO_OSG_HPP
#define IO_OSG_HPP

#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Building.hpp"
#include <map>

namespace io
{
    void display(std::map<int,std::vector<Building> >& exp_bldgs, std::map<int,Lot>& lots);

//   for test
    void display(double x1[4],double y1[4],double x2[4],double y2[4]);

    template<typename T>
    void display(const T& t1, const T& t2)
    {
        double x1[4],y1[4],x2[4],y2[4];
        for(int i=0;i<4;++i)
        {
            x1[i] = t1.bottom().point(i).x();
            y1[i] = t1.bottom().point(i).y();
            x2[i] = t2.bottom().point(i).x();
            y2[i] = t2.bottom().point(i).y();
        }

        display(x1,y1,x2,y2);
    }
}

#endif // IO_OSG_HPP
