#ifndef BLDG_HPP
#define BLDG_HPP

#include "sfcgal.hpp"
#include "Lot.hpp"
#include <SFCGAL/algorithm/extrude.h>
#include <SFCGAL/algorithm/area.h>
#include "io/convert.hpp"



class Bldg{

public:
    Bldg(){}
    Bldg(OGRPolygon& f, double h):_footprint(io::OGR2SFCGAL(&f)),_h(h){}

    Bldg(Polygon& f, double h):_footprint(f),_h(h){}

    Bldg(Polygon& f, double h, Lot* lot):_footprint(f),_h(h),_idLot(lot->id()){}


    ~Bldg(){}

    void extrude()
    {
        _block = *((Solid*)(SFCGAL::algorithm::extrude(_footprint,0,0,_h).release()));
    }

    void translate(double x0,double y0)
    {
        LineString ring;
        int n=_footprint.exteriorRing().numPoints();
        for(int i=0;i<n;++i)
        {
            double x = CGAL::to_double(_footprint.exteriorRing().pointN(i).x())-x0;
            double y = CGAL::to_double(_footprint.exteriorRing().pointN(i).y())-y0;
            Point pt(x,y);
            ring.addPoint(pt);
        }
        _footprint.setExteriorRing(ring);
    }

public:
    Polygon _footprint;
    double _h;
    int _idLot;
    Solid _block;

};


#endif
