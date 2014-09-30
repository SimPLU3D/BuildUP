#ifndef PLU_BINARY_ENERGY_HPP
#define PLU_BINARY_ENERGY_HPP

#include "rjmcmc/rjmcmc/energy/energy.hpp"
#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Expression.hpp"
#include <map>

template<typename Value = double>
class plu_binary_heightDiff: public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t1, const T &t2) const
    {
//        Value d = t1.distance2cuboid(t2);
//
//        if(geometry::do_intersect(t1,t2))
//            d = -d;
//
//        Value hMax = t1.h()>t2.h()? t1.h():t2.h();
//
//        if(d>=hMax)
//            return 0;


        Value hDiff = std::abs(t1.h()-t2.h());

        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("hDiff"),hDiff));

        return _eRej*(_lot->ruleEnergy(RuleType::HeightDiff))->energy(varValue);

    }

    plu_binary_heightDiff(Lot* lot, double eRej):_lot(lot),_eRej(eRej) {}

private:
    Lot* _lot;
    double _eRej;
};

template<typename Value = double>
class plu_binary_distPair : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline double operator()(const T &t1,const T &t2) const
    {
        Value d = t1.distance2cuboid(t2);
        if(d!=d || std::isinf(d))
        {
            std::cout<<"binary distance nan or inf\n";
            return _eRej;
        }
        if(geometry::do_intersect(t1,t2))
            d = -d;

        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("dPair"),d));

        Value hMax = t1.h()>t2.h()? t1.h():t2.h();
        return _eRej*(_lot->ruleEnergy(RuleType::DistPair))->energy(varValue,hMax);
    }

    plu_binary_distPair (Lot* lot, Value eRej):_lot(lot),_eRej(eRej) {}

private:
    Lot *_lot;
    Value _eRej;
};


#endif // PLU_BINARY_ENERGY_HPP
