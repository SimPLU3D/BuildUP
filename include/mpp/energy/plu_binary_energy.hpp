#ifndef PLU_BINARY_ENERGY_HPP
#define PLU_BINARY_ENERGY_HPP

#include "plu/Lot.hpp"


//template<typename Value = double>
//class plu_binary_intersection : public rjmcmc::energy<Value>
//{
//public:
//    typedef Value result_type;
//
//    template<typename T>
//    inline result_type operator()(const T &t, const T &u) const
//    {
//        return 0;
//        if(!geometry::do_intersect(t,u))
//            return 0;
//
//        result_type a = geometry::intersection_area(t,u);
//        if(a!=a || std::isinf(a))
//            return _eRej;
//
//        std::map<Var,double> varValue;
//        varValue.insert(std::make_pair(Var("overlap"),a*a));
////        double e = _eRej*(_lot->_rules[RT_Overlap])->energy(varValue);
////        std::cout<<"overlap area "<<a<<" energy"<< e<<"\n";
////        return e;
//
//        return _eRej*(_lot->_rules[RT_Overlap])->energy(varValue);
//
//    }
//
//    plu_binary_intersection(Lot* lot, double eRej):_lot(lot),_eRej(eRej){}
//
//private:
//    Lot* _lot;
//    double _eRej;
//};

template<typename Value = double>
class plu_binary_distance : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t1,const T &t2) const
    {
        //return 0;


        Value d = std::sqrt(t1.squared_distance(t2));
        if(d!=d || std::isinf(d))
            return _eRej;

        if(geometry::do_intersect(t1,t2))
            d = -d;

        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("dPair"),d));

//        if(d<0)
//        {
//        std::cout<<"dPair "<<d<<"\n";
//        std::cout<<"ePair "<<_eRej*(_lot->_rules[RT_DistancePairwise])->energy(varValue)<<"\n";
//        }
//        test zone1 /////////////////////////////////////////////////////////:
        return _eRej*(_lot->_rules[RT_DistancePairwise])->energy(varValue);

        //test zone2 /////////////////////////////////////////////////////////:
        Value hMin = t1.h<t2.h? t1.h:t2.h;
        Value hMax = t1.h>t2.h? t1.h:t2.h;

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;

        Literal l1(Constant(hMax*0.5));
        Literal l2(Constant(std::max(hMax-3,8.0)));
        Literal l3(Constant(std::max(hMin-3,8.0)));

        Constraint* c1 = new AC(Var("dPair"),RL_Greater,l1);
        Constraint* c2 = new AC(Var("dPair"),RL_Greater,l2);
        Constraint* c3 = new AC(Var("dPair"),RL_Greater,l3);

        Constraint* c12 = new CompConstraint(c1,c2,RL_Or);
        Constraint* c123 = new CompConstraint(c12,c3,RL_Or);

        std::vector<Constraint*> cs;
        cs.push_back(c123);

        RuleStatic rule(RT_DistancePairwise,cs);
        return _eRej*rule.energy(varValue);

    }

    plu_binary_distance (Lot* lot, Value eRej):_lot(lot),_eRej(eRej){}

private:
    Lot *_lot;
    Value _eRej;
};


#endif // PLU_BINARY_ENERGY_HPP
