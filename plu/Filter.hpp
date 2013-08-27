#ifndef FILTER_HPP
#define FILTER_HPP

#include "geometry/Rectangle_2.hpp"
#include <gdal/ogrsf_frmts.h>
#include <SFCGAL/Point.h>
#include <SFCGAL/LineString.h>
#include <SFCGAL/Polygon.h>
#include <SFCGAL/algorithm/intersects.h>
#include <SFCGAL/algorithm/intersection.h>
#include <SFCGAL/algorithm/area.h>
#include <math.h>
#include <CGAL/number_utils.h>

class Filter{

public:
    Filter(OGRPolygon* ply):polygon(ply){}
    Filter(const Filter& o){polygon=o.polygon;}
    ~Filter(){}

    template <typename T>
    bool operator () (T & obj) const
    {
        //    std::cout << "Filter";
        geometry::Rectangle_2<geometry::Simple_cartesian<double>> a = obj.rect_2;

        if(a.is_degenerate())
            return true;

        OGRPolygon plyA;
        OGRLinearRing ringA;
        for(int i=0;i<5;i++)
        {
            if(a.point(i).x()!= a.point(i).x() || a.point(i).y()!= a.point(i).y())
                return false;
            for(int j=i+1;j<4;j++)
                if(a.point(i)[0] == a.point(j)[0] && a.point(i)[1] == a.point(j)[1])
                    return false;

            double x = CGAL::to_double(a.point(i).x());
            double y = CGAL::to_double(a.point(i).y());
            if(isinf(x) || isinf(y))
                return false;
            ringA.addPoint(x,y);
            //std::cout << x << "," << y << "\t" << std::flush;
        }
        plyA.addRing(&ringA);

        //std::cout << "h" << obj.h << "\t" << std::flush;
        //std::cout << "ok" << plyA.Within(polygon) << std::endl;

        return (plyA.Within(polygon));
    }

public:
    OGRPolygon* polygon;


};

#endif // FILTER_HPP
