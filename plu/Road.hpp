#ifndef ROAD_HPP
#define ROAD_HPP

//#include "sfcgalTypes.hpp"
//#include <gdal/ogrsf_frmts.h>

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "Rule.hpp"
#include <math.h>

class RoadSeg
{
public:
    typedef geometry::Simple_cartesian<double> K;
    typedef typename K::Segment_2 Segment_2;
//    typedef geometry::Rectangle_2 Rectangle_2;

    RoadSeg(Segment_2& seg,double width,RuleRoad& rule)
    :_seg(seg), _width(width),_rule(rule)
    {//make_surface();
    }

    ~RoadSeg(){}

//private:
//    void make_surface()
//    {
//        _surface.center(geometry::midpoint(_seg.target(),_seg.source()));
//        _surface.normal((_seg.target()-_seg.source())/2);
//        _surface.ratio(sqrt(_seg.squared_length())/_width);
//    }

public:
    Segment_2 _seg;
    double _width;
    RuleRoad _rule;

   // Rectangle_2 _surface;
    Segment_2 _edge_left;
    Segment_2 _edge_right;


};
#endif // ROAD_HPP
