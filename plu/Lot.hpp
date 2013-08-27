#ifndef LOT_HPP
#define LOT_HPP

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "Rule.hpp"
//#include <SFCGAL/all.h>
#include <gdal/ogrsf_frmts.h>

typedef geometry::Simple_cartesian<double> K;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;


class Lot{
    public:
        Lot(){}
        Lot(OGRPolygon* ply, RuleLot& rule, Iso_rectangle_2& box)
        : _polygon((OGRPolygon*)(ply->clone())), _rule(rule),_box2d(box),_area(ply->get_Area()){}
        ~Lot(){_polygon->empty();}

    public:
        OGRPolygon* _polygon;
        RuleLot _rule;
        Iso_rectangle_2 _box2d;
        double _area;
};


#endif // LOT_HPP
