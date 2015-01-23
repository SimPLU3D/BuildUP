#ifndef BLDG_HPP
#define BLDG_HPP

#include <gdal/ogrsf_frmts.h>
#include "Procedural.hpp"

class Building
{
    OGRPolygon* _footprint;
    double _length;
    double _width;
    double _height;
    int _idLot;


public:
    inline Building():_footprint(0) {}

    inline Building(OGRPolygon* ply,double l,double w,double h,int idLot=-1)
    :_footprint((OGRPolygon*)(ply->clone()))
    ,_length(l),_width(w),_height(h),_idLot(idLot) {}

    inline Building(const Building& o)
    {
        _footprint = (OGRPolygon*)(o._footprint->clone());
        _length = o._length;
        _width = o._width;
        _height = o._height;
        _idLot = o._idLot;
    }

    inline ~Building(){ if(_footprint) _footprint->empty();}

    inline void translate_footprint(double dx,double dy)
    {
        OGRLinearRing* ring = _footprint->getExteriorRing();
        for(int i=0;i<ring->getNumPoints();++i)
            ring->setPoint(i,ring->getX(i)+dx,ring->getY(i)+dy,ring->getZ(i));
    }

    //gets
    inline OGRPolygon* footprint() const {return _footprint;}
    inline double height() const {return _height;}
    inline int idLot() const {return _idLot;}

    //extrude 3D box
    OGRMultiPolygon* extrude_box() const;

    //extrude envelope; roof and walls are stored separately; only support flat roof for now
    Surface* extrude_envelope() const;

    //extrude lod3 by procedural modeling
    Surface* extrude_lod3(double hFloor, double lenHasWin, double lenUnit) const;

};

#endif
