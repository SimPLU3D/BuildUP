#ifndef BLDG_HPP
#define BLDG_HPP

#include "sfcgalTypes.hpp"
#include "Lot.hpp"
#include <SFCGAL/algorithm/extrude.h>
#include <SFCGAL/algorithm/area.h>


class Bldg{

public:
    Bldg(){}
    Bldg(Polygon& f, double h, Lot* lot):_footprint(f),_h(h),_lot(lot)
    {
        _ces = SFCGAL::algorithm::area(_footprint)/_lot->_area;
        _h = h>lot->_rule._hMin ? h:lot->_rule._hMin;
        _nf = _h/(lot->_rule._hMin);
        _cos = _ces*_nf;
    }

    ~Bldg(){}

    void extrude()
    {
        _block = *((Solid*)(SFCGAL::algorithm::extrude(_footprint,0,0,_h).release()));
    }


public:
    Polygon _footprint;
    double _h;
    Lot* _lot;

    double _ces;
    double _cos;
    int _nf; //num of floor

    Solid _block;

    //MultiPolygon _wall;
    //MultiPolygon _roof;

};



#endif
