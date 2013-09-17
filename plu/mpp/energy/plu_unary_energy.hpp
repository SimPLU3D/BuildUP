#ifndef PLU_UNARY_ENERGY_HPP
#define PLU_UNARY_ENERGY_HPP

//#include "geometry/intersection/Rectangle_2_polygon_intersection.hpp"
#include "Lot.hpp"
#include <math.h>

#ifndef Pi
#define Pi 3.14159265358979323846
#endif
template<typename Value = double>
class plu_unary_ces : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t) const
    {
        return t.area()/_lot->_area;
    }


    plu_unary_ces(Lot* lot) : _lot(lot) {}

private:
    Lot* _lot;
};

template<typename Value = double>
class plu_unary_cos : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t) const
    {
        int nFloor = t.h/_lot->_rule._hMin;
        return t.area()* nFloor/_lot->_area;
    }

    plu_unary_cos(Lot* lot) : _lot(lot) {}

private:
    Lot* _lot;
};


template<typename Value = double>
class plu_unary_border : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t) const
    {
        //distance between the object and the normal of the lot (eg parallel to the longest edge)
        result_type d = t.distance2line(_lot->_mainEdge.source(),_lot->_mainEdge.target());
        return d<_lot->_rule._distMinUr? _eRej:0;
    }

   plu_unary_border(Lot* lot,result_type eRej) : _lot(lot),_eRej(eRej) {}


private:
    Lot* _lot;
    result_type _eRej;
};


#endif // PLU_UNARY_ENERGY_HPP
