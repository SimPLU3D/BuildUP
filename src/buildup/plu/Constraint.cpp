#include "buildup/plu/Constraint.hpp"


EnergyPLU* CompConstraint::toEnergy(double x,double acceptScale,EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType)
{
    EnergyPLU* e;
    if(isForSameVar())
        e = new EnergyPiecewise(getVar(),toInterval(x),acceptScale,acceptType,penaltyScale,penaltyType);
    else
        e = new EnergyComposite(_leftChild->toEnergy(x,acceptScale,acceptType,penaltyScale,penaltyType)
                                ,_rightChild->toEnergy(x,acceptScale,acceptType,penaltyScale,penaltyType),_r);

    return e;
}

CompConstraint::Intervals CompConstraint::toInterval(double x)
{
    if(!isForSameVar())
        std::cerr<<"error: can not convert to intervals for different variables";

    switch(_r)
    {
    case Relation::And:
        return intervalAND(_leftChild->toInterval(x),_rightChild->toInterval(x));
    case Relation::Or:
        return intervalOR(_leftChild->toInterval(x),_rightChild->toInterval(x));
    default:
        std::cerr<<"error: wrong logic";
        exit(1);
    }
}

CompConstraint::Intervals CompConstraint::intervalOR(Intervals is1,Intervals is2)
{
    Intervals out;
    IntervalMap iMap;
    Intervals::iterator itV;
    for(itV=is1.begin(); itV!=is1.end(); ++itV)
        iMap += std::make_pair(*itV,1);
    for(itV=is2.begin(); itV!=is2.end(); ++itV)
        iMap += std::make_pair(*itV,1);

    Intervals overlaps;
    IntervalMap::iterator itMap;
    for(itMap=iMap.begin(); itMap!=iMap.end(); ++itMap)
        if(itMap->second == 2)
            overlaps.push_back(itMap->first);
    for(itV=overlaps.begin(); itV!=overlaps.end(); ++itV)
        iMap += std::make_pair(*itV,-1);

    for(itMap=iMap.begin(); itMap!=iMap.end(); ++itMap)
        out.push_back(itMap->first);

    return out;
}

CompConstraint::Intervals CompConstraint::intervalAND(Intervals is1,Intervals is2)
{
    Intervals out;
    IntervalMap iMap;
    Intervals::iterator itV;
    for(itV=is1.begin(); itV!=is1.end(); ++itV)
        iMap += std::make_pair(*itV,1);
    for(itV=is2.begin(); itV!=is2.end(); ++itV)
        iMap += std::make_pair(*itV,1);

    IntervalMap::iterator itMap;
    for(itMap = iMap.begin(); itMap!=iMap.end(); ++itMap)
        if(itMap->second == 2)
            out.push_back(itMap->first);
    return out;
}

