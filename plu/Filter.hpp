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
#include "Lot.hpp"

#ifndef Pi
#define Pi 3.14159265358979323846
#endif

class Filter{

public:
    Filter(Lot* lot):_lot(lot){}
    Filter(const Filter& o){_lot=o._lot;}
    ~Filter(){}

    template <typename T>
    int operator () (T & obj) const
    {

        geometry::Rectangle_2<geometry::Simple_cartesian<double>> a = obj.rect_2;

        if(a.is_degenerate())
            return 1;


        OGRPolygon plyA;
        OGRLinearRing ringA;
        for(int i=0;i<5;i++)
        {
            if(a.point(i).x()!= a.point(i).x() || a.point(i).y()!= a.point(i).y())
                return 0;
            for(int j=i+1;j<4;j++)
                if(a.point(i)[0] == a.point(j)[0] && a.point(i)[1] == a.point(j)[1])
                    return 0;

            double x = CGAL::to_double(a.point(i).x());
            double y = CGAL::to_double(a.point(i).y());
            if(isinf(x) || isinf(y))
                return 0;
            ringA.addPoint(x,y);
            //std::cout << x << "," << y << "\t" << std::flush;
        }
        plyA.addRing(&ringA);

        //std::cout << "h" << obj.h << "\t" << std::flush;
        //std::cout << "ok" << plyA.Within(polygon) << std::endl;

        if(!plyA.Within(_lot->_polygon))
            return 0;

//return 1;
        //check size
        if(a.area()<_lot->_rule._sizeMin)
            return 0;

        //check angle
        double thetaMax = _lot->_rule._thetaMax;
        double theta = acos(_lot->_normal*a.normal()/sqrt(_lot->_normal.squared_length()*a.normal().squared_length()));

        double theta1=abs(90-theta*180/Pi);

        if(theta1<=thetaMax)
            return 1;
        if(theta1>=(90-thetaMax))
            return 1;

        return 0;

    }

public:
   // OGRPolygon* polygon;
    Lot* _lot;


};

#endif // FILTER_HPP
