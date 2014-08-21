#ifndef PLU_UNARY_ENERGY_HPP
#define PLU_UNARY_ENERGY_HPP

#include "Lot.hpp"
#include <math.h>


#ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI


template<typename Value = double>
class plu_unary_distBorder : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;


    template<typename T>
    inline result_type operator()(const T &t) const
    {

        std::map< std::string,double > dist;
        _lot->dist2borders(t,dist);


        std::map<Var,double> var_value;
        std::map< std::string,double >::iterator it;
        for(it=dist.begin();it!=dist.end();++it)
        {
            if(it->second!=it->second || isinf(it->second))
                return _eRej*3;

            var_value.insert(std::make_pair(Var("d"+it->first),it->second));
        }

        result_type eFront=0,eSide=0,eBack=0;

        if(_lot->hasRule(RuleType::DistFront))
            eFront = (_lot->ruleEnergy(RuleType::DistFront))->energy(var_value,t.h());

        if(_lot->hasRule(RuleType::DistSide))
            eSide = (_lot->ruleEnergy(RuleType::DistSide))->energy(var_value,t.h());

        if(_lot->hasRule(RuleType::DistBack))
            eBack = (_lot->ruleEnergy(RuleType::DistBack))->energy(var_value,t.h());

        return (eFront+eSide+eBack)*_eRej;
    }

   plu_unary_distBorder(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej){}

private:
    Lot* _lot;
    Value _eRej;
};

#endif // PLU_UNARY_ENERGY_HPP
