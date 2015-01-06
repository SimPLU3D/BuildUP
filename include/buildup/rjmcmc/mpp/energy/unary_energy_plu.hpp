#ifndef PLU_UNARY_ENERGY_HPP
#define PLU_UNARY_ENERGY_HPP

#include "rjmcmc/rjmcmc/energy/energy.hpp"
#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Expression.hpp"
#include <map>
#include <string>

template<typename Value = double>
class plu_unary_distBorder : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;


    template<typename T>
    inline result_type operator()(const T &t) const
    {
//std::cout<<"unary energy\n";
        std::map< std::string,double > dist,hasWindow;
        _lot->dist2borders(t,dist,hasWindow);


        std::map<Var,double> var_value;
        std::map< std::string,double >::iterator it;
        for(it=dist.begin(); it!=dist.end(); ++it)
        {
            if(it->second!=it->second || isinf(it->second))
                return _eRej*3;

            var_value.insert(std::make_pair(Var("d"+it->first),it->second));
        }

        for(it=hasWindow.begin();it!=hasWindow.end();++it)
            var_value.insert(std::make_pair(Var("hasWindow"+it->first),it->second));

        var_value.insert(std::make_pair(Var("h"),t.h()));
        result_type eFront=0,eSide=0,eBack=0;

        if(_lot->hasRule(RuleType::DistFront))
            eFront = (_lot->ruleEnergy(RuleType::DistFront))->energy(var_value);

        if(_lot->hasRule(RuleType::DistSide))
            eSide = (_lot->ruleEnergy(RuleType::DistSide))->energy(var_value);

        if(_lot->hasRule(RuleType::DistBack))
            eBack = (_lot->ruleEnergy(RuleType::DistBack))->energy(var_value);

        return (eFront+eSide+eBack)*_eRej;
    }


    inline result_type operator()(double x,double h) const
    {
        std::map<Var,double> var_value;
        std::map< std::string,Border* >::iterator it,bbegin,bend;
        bbegin = _lot->name_borders().begin();
        bend = _lot->name_borders().end();
        for(it=bbegin;it!=bend;++it)
        {
            var_value.insert(std::make_pair(Var("d"+it->first),x));
            var_value.insert(std::make_pair(Var("hasWindow"+it->first),1.));
        }
        var_value.insert(std::make_pair(Var("h"),_lot->ruleGeom()->hMax()));

        double eFront=0,eSide=0,eBack=0;
        if(_lot->hasRule(RuleType::DistFront))
            eFront = (_lot->ruleEnergy(RuleType::DistFront))->energy(var_value);

        if(_lot->hasRule(RuleType::DistSide))
            eSide = (_lot->ruleEnergy(RuleType::DistSide))->energy(var_value);

        if(_lot->hasRule(RuleType::DistBack))
            eBack = (_lot->ruleEnergy(RuleType::DistBack))->energy(var_value);


        return _eRej*std::max(std::max(eFront,eSide),eBack);
    }

    plu_unary_distBorder(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej) {}

private:
    Lot* _lot;
    Value _eRej;
};

#endif // PLU_UNARY_ENERGY_HPP
