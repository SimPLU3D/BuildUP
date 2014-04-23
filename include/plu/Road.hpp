#ifndef ROAD_HPP
#define ROAD_HPP

//#include "sfcgalTypes.hpp"
//#include <gdal/ogrsf_frmts.h>

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "Rule.hpp"
#include <math.h>
#include <gdal/ogrsf_frmts.h>


class Road
{
public:
    typedef geometry::Simple_cartesian<double> K;
    typedef typename K::Segment_2 Segment_2;
//    typedef geometry::Rectangle_2 Rectangle_2;

    Road(){_polyline = new OGRLineString;}

    Road(int id,OGRLineString* polyline,double w)
    :_id(id),_polyline((OGRLineString*)(polyline->clone())),_width(w)
    {//_polygon = _polyline.Buffer(_width/2);
    }

    Road(const Road& o)
    {
        _id = o._id;
        _polyline = (OGRLineString*)(o._polyline->clone());
        _width = o._width;
        //_polygon = (OGRPolygon*)(o._polygon.clone());

    }
    ~Road(){_polyline->empty();}

//private:
//    void make_surface()
//    {
//        _surface.center(geometry::midpoint(_seg.target(),_seg.source()));
//        _surface.normal((_seg.target()-_seg.source())/2);
//        _surface.ratio(sqrt(_seg.squared_length())/_width);
//    }


    void translate(double x0,double y0)
    {
        int n=_polyline->getNumPoints();
        for(int i=0;i<n;++i)
        {
            double x = _polyline->getX(i)-x0;
            double y = _polyline->getY(i)-y0;
            _polyline->setPoint(i,x,y);
        }
    }


public:
    int _id;
    OGRLineString* _polyline;
    double _width;
    //OGRPolygon* _polygon;


};



#endif // ROAD_HPP
