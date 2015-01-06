#ifndef PLU_BINARY_ENERGY_HPP
#define PLU_BINARY_ENERGY_HPP

#include "rjmcmc/rjmcmc/energy/energy.hpp"
#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Expression.hpp"
#include <map>
#include "buildup/rjmcmc/geometry/Cuboid.hpp"
#include "buildup/viewer/osg.hpp"

template<typename Value = double>
class plu_binary_heightDiff: public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t1, const T &t2) const
    {
        if(geometry::do_intersect(t1,t2))
            return _eRej;

//        Value d = t1.distance2cuboid(t2);
//
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

    inline int geq(double a,double b,double epsilon = 0.001) const
    {
        return (a>b || std::abs(a-b)<epsilon);
    }

    template<typename T>
    inline double operator()(const T &t1,const T &t2) const
    {
        if(geometry::do_intersect(t1,t2))
             return _eRej;

        std::map<Var,double> varValue;
        double dPair=-1;

        if(!_lot->ruleEnergy(RuleType::DistPair)->isConditional())
            dPair = t1.distance2cuboid(t2);
        else
        {
            int hasWindowPair,hasWindowHigh,hasWindowLow;
            dPair = t1.distance2cuboid(t2, _lot->lengthHasWindow(),hasWindowPair,hasWindowHigh,hasWindowLow);
            varValue.insert(std::make_pair(Var("hasWindowPair"),hasWindowPair));
            varValue.insert(std::make_pair(Var("hasWindowHigh"),hasWindowHigh));
            varValue.insert(std::make_pair(Var("hasWindowLow"),hasWindowLow));
        }

//        if(dPair!=dPair || std::isinf(dPair))
//        {
//            std::cout<<"binary distance nan or inf\n";
//            return _eRej;
//        }
        if(geq(dPair,t1.h()) && geq(dPair,t2.h()) )
            return 0;

        varValue.insert(std::make_pair(Var("dPair"),dPair));
        double hHigh = geq(t1.h(),t2.h())? t1.h():t2.h();
        double hLow = geq(t1.h(),t2.h())? t2.h():t1.h();
        varValue.insert(std::make_pair(Var("hHigh"),hHigh));
        varValue.insert(std::make_pair(Var("hLow"),hLow));
        return _eRej*(_lot->ruleEnergy(RuleType::DistPair))->energy(varValue);
    }

    plu_binary_distPair (Lot* lot, Value eRej):_lot(lot),_eRej(eRej) {}

private:
    Lot *_lot;
    Value _eRej;
};


#endif // PLU_BINARY_ENERGY_HPP
