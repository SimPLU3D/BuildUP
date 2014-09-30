#ifndef BLDG_HPP
#define BLDG_HPP

#include <gdal/ogrsf_frmts.h>

class Building
{
    OGRPolygon* _footprint;
    double _h;
    int _idLot;

public:
    inline Building():_footprint(0) {}

    inline Building(OGRPolygon* ply,double h,int idLot=-1)
    :_footprint((OGRPolygon*)(ply->clone())),_h(h),_idLot(idLot) {}

    inline Building(const Building& o)
    {
        _footprint = (OGRPolygon*)(o._footprint->clone());
        _h = o._h;
        _idLot = o._idLot;
    }

    inline ~Building(){ if(_footprint) _footprint->empty();}

    //gets
    inline OGRPolygon* footprint() const {return _footprint;}
    inline double height() const {return _h;}
    inline int idLot() const {return _idLot;}

    //extrude 3D box
    OGRMultiPolygon* extrude() const;

};

#endif
