#ifndef PLU_GLOBAL_ENERGY_HPP_INCLUDED
#define PLU_GLOBAL_ENERGY_HPP_INCLUDED

//#include "geometry/intersection/Cuboid_bldg_intersection.hpp"
#include "plu/Lot.hpp"

template<typename Value = double>
class plu_global_ces : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Container>
    result_type operator()(const Container &c) const
    {
        double area=0;
        typename Container::const_iterator it;
        for(it=c.begin();it!=c.end();++it)
            area+=(*it).area();
        double ces = area/_lot->_area;
        double MaxCES = _lot->_rule._cesMax;
        return MaxCES >= ces? (MaxCES-ces)*_coef:_eRej;
    }

    plu_global_ces(Lot* lot,Value eRej, Value coef) : _lot(lot),_eRej(eRej),_coef(coef){}

private:
    Lot* _lot;
    Value _eRej;
    Value _coef;
};

template<typename Value = double>
class plu_global_cos : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Container>
    result_type operator()(const Container &c) const
    {
        double areaF=0,hF = _lot->_rule._hMin;
        typename Container::const_iterator it,it2;
        for(it=c.begin();it!=c.end();++it)
            areaF+=(*it).area()*(int)((*it).h/hF);
        double cos = areaF/_lot->_area;
        double MaxCOS = _lot->_rule._cosMax;
        return MaxCOS >= cos? (MaxCOS-cos)*_coef:_eRej;
    }

    plu_global_cos(Lot* lot,Value eRej, Value coef) : _lot(lot),_eRej(eRej),_coef(coef){}

private:
    Lot* _lot;
    Value _eRej;
    Value _coef;
};

#endif // PLU_GLOBAL_ENERGY_HPP_INCLUDED
