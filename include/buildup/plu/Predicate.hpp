#ifndef PREDICATE_HPP_INCLUDED
#define PREDICATE_HPP_INCLUDED

#include <gdal/ogrsf_frmts.h>
#include <rjmcmc/geometry/Rectangle_2.hpp>

class Predicate
{

public:
    Predicate(OGRPolygon* ply):m_polygon(ply) {}
    ~Predicate() {}

    template <typename T>
    int operator () (T & obj) const
    {
        //whether the object is inside the polygon

        geometry::Rectangle_2<geometry::Simple_cartesian<double>> a = obj.bottom();

        OGRPolygon plyA;
        OGRLinearRing ringA;
        for(int i=0; i<5; i++)
        {
            if(a.point(i).x()!= a.point(i).x() || a.point(i).y()!= a.point(i).y())
                return 0;
            for(int j=i+1; j<4; j++)
                if(a.point(i)[0] == a.point(j)[0] && a.point(i)[1] == a.point(j)[1])
                    return 0;

            double x = a.point(i).x();
            double y = a.point(i).y();
            if(isinf(x) || isinf(y))
                return 0;
            ringA.addPoint(x,y);
        }
        plyA.addRing(&ringA);

        if(!plyA.Within(m_polygon))
            return 0;

        return 1;

    }

public:
    OGRPolygon* m_polygon;
};

#endif // PREDICATE_HPP_INCLUDED
