#ifndef PLU_UNARY_ENERGY_HPP
#define PLU_UNARY_ENERGY_HPP

//#include "geometry/intersection/Rectangle_2_polygon_intersection.hpp"
#include "plu/Lot.hpp"
#include <math.h>

#ifndef Pi
#define Pi 3.14159265358979323846
#endif



template<typename Value = double>
class plu_unary_border_d : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;


    template<typename T>
    inline result_type operator()(const T &t) const
    {
        //return 0;
        std::map< BorderType,std::map<int,double> > dist;
        _lot->dist2borders(t,dist);

        std::map< BorderType,std::map<int,double> >::iterator it1;
        std::map<int,double>::iterator it2;
        for(it1=dist.begin();it1!=dist.end();++it1)
            for(it2=it1->second.begin();it2!=it1->second.end();++it2)
                if(it2->second!=it2->second || isinf(it2->second))
                    return _eRej*3;


        //test zone1 //////////////////////////////////////////////////////////////////////////:
        result_type dFront,dBack,dSide1,dSide2;
        result_type eFront,eSide,eBack;

        dFront = dist[FrontBorder][0];
        dSide1 = dist[SideBorder][0];
        dSide2 = dist[SideBorder][1];
        dBack = dist[BackBorder][0];

        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("dFront"),dFront));
        varValue.insert(std::make_pair(Var("dSide1"),dSide1));
        varValue.insert(std::make_pair(Var("dSide2"),dSide2));
        varValue.insert(std::make_pair(Var("dBack"),dBack));


//        result_type dFront_obj = t.distance2line_obj(_lot->getRefSeg()); //center to front border
//        double wBand = 20;
//        if(dFront_obj<=wBand || dFront<wBand/2)
//            eSide= (_lot->_rules[RT_DistanceSide])->energy(varValue,t.h,0);
//
//        else
//            eSide= (_lot->_rules[RT_DistanceSide])->energy(varValue,t.h,1);

        eSide = (_lot->_rules[RT_DistanceSide])->energy(varValue,t.h,0);
        eFront = (_lot->_rules[RT_DistanceFront])->energy(varValue);
        eBack = (_lot->_rules[RT_DistanceBack])->energy(varValue);

        return (eFront+eSide+eBack)*_eRej;

//        //test zone2 //////////////////////////////////////////////////////////////////////////:
//        result_type dFront1,dFront2,dSide1,dSide2;
//        result_type eFront,eSide;
//
//        dFront1 = dist[FrontBorder][0];
//        dFront2 = dist[FrontBorder][2];
//        dSide1 = dist[SideBorder][0];
//        dSide2 = dist[SideBorder][1];
//
//        std::map<Var,double> varValue;
//        varValue.insert(std::make_pair(Var("dFront1"),dFront1));
//        varValue.insert(std::make_pair(Var("dFront2"),dFront2));
//        varValue.insert(std::make_pair(Var("dSide1"),dSide1));
//        varValue.insert(std::make_pair(Var("dSide2"),dSide2));
//
//
//        eFront = (_lot->_rules[RT_DistanceFront])->energy(varValue);
//        eSide = (_lot->_rules[RT_DistanceSide])->energy(varValue,t.h);
//
//
//
//        return (eFront+eSide)*_eRej;

    }

   plu_unary_border_d(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej) {}


private:
    Lot* _lot;
    Value _eRej;

};


#endif // PLU_UNARY_ENERGY_HPP
