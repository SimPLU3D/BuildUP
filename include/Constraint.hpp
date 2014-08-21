#ifndef CONSTRAINT_HPP_INCLUDED
#define CONSTRAINT_HPP_INCLUDED

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>
#include "Expression.hpp"
#include "Energy.hpp"


class Constraint{
public:
    Constraint(){}
    virtual ~Constraint(){}

    typedef boost::icl::continuous_interval<double> Interval;
    typedef std::vector<Interval> Intervals;
    typedef boost::icl::interval_map<double,int> IntervalMap;
    typedef std::numeric_limits<double> Limit;


    virtual Var getVar(void)=0;
    virtual bool isForVar(Var)=0;
    virtual bool isForSameVar(void)=0;
    virtual Intervals toInterval(double)=0;

    virtual EnergyPLU* toEnergy(double x,double acceptScale, EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType)=0;

};

template<typename E> //E is an expression type
class AtomConstraint:public Constraint{
public:
    AtomConstraint(Var v,Relation r,E expr):_var(v),_r(r),_expr(expr){}

    EnergyPLU* toEnergy(double x,double acceptScale, EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType)
    {return new EnergyPiecewise(_var,toInterval(x),acceptScale,acceptType,penaltyScale,penaltyType);}

private:
    inline Var getVar(void){return _var;}
    inline bool isForVar(Var v){return _var==v;}
    inline bool isForSameVar(void){return true;}

    Intervals toInterval(double x)
    {
        Intervals intervals;
        double y = _expr(x);
        switch (_r)
        {
            case Relation::Equal:
                intervals.push_back( Interval::closed(y,y));
                break;
            case Relation::Greater:
                intervals.push_back( Interval::open(y,Limit::infinity()));
                break;
            case Relation::GreaterEqual:
                intervals.push_back( Interval::right_open(y,Limit::infinity()));
                break;
            case Relation::Less: //plu constraints are >=0 implicitly
                intervals.push_back( Interval::right_open(0,y));
                break;
            case Relation::LessEqual:
                intervals.push_back( Interval::closed(0,y));
                break;
            default:
                std::cerr<<"error: wrong relation type";
        }
        return intervals;
    }

private:
    Var _var;
    Relation _r;
    E _expr;
};




class CompConstraint: public Constraint{

public:
    CompConstraint(Constraint* lc, Constraint* rc,Relation r):_leftChild(lc),_rightChild(rc),_r(r){}
    ~CompConstraint(){if (_leftChild!=NULL) delete _leftChild; if(_rightChild!=NULL) delete _rightChild;}

    EnergyPLU* toEnergy(double x,double acceptScale,EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType)
    {
        EnergyPLU* e;
        if(isForSameVar())
            e = new EnergyPiecewise(getVar(),toInterval(x),acceptScale,acceptType,penaltyScale,penaltyType);
        else
            e = new EnergyComposite(_leftChild->toEnergy(x,acceptScale,acceptType,penaltyScale,penaltyType)
            ,_rightChild->toEnergy(x,acceptScale,acceptType,penaltyScale,penaltyType),_r);

        return e;
    }

private:
    inline Var getVar(void){return _leftChild->getVar();}//leftest var of its children
    inline bool isForVar(Var v){return _rightChild->isForVar(v) && _leftChild->isForVar(v);}
    inline bool isForSameVar(void){return _leftChild->isForVar(_leftChild->getVar()) && _rightChild->isForVar(_leftChild->getVar());}

    Intervals toInterval(double x)
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

    Intervals intervalOR(Intervals is1,Intervals is2)
    {
        Intervals out;
        IntervalMap iMap;
        Intervals::iterator itV;
        for(itV=is1.begin();itV!=is1.end();++itV)
            iMap += std::make_pair(*itV,1);
        for(itV=is2.begin();itV!=is2.end();++itV)
            iMap += std::make_pair(*itV,1);

        Intervals overlaps;
        IntervalMap::iterator itMap;
        for(itMap=iMap.begin();itMap!=iMap.end();++itMap)
            if(itMap->second == 2)
                overlaps.push_back(itMap->first);
        for(itV=overlaps.begin();itV!=overlaps.end();++itV)
            iMap += std::make_pair(*itV,-1);

        for(itMap=iMap.begin();itMap!=iMap.end();++itMap)
            out.push_back(itMap->first);

        return out;

    }
    Intervals intervalAND(Intervals is1,Intervals is2)
    {
        Intervals out;
        IntervalMap iMap;
        Intervals::iterator itV;
        for(itV=is1.begin();itV!=is1.end();++itV)
            iMap += std::make_pair(*itV,1);
        for(itV=is2.begin();itV!=is2.end();++itV)
            iMap += std::make_pair(*itV,1);

        IntervalMap::iterator itMap;
        for(itMap = iMap.begin();itMap!=iMap.end();++itMap)
            if(itMap->second == 2)
                out.push_back(itMap->first);
        return out;
    }

private:
    Constraint* _leftChild;
    Constraint* _rightChild;
    Relation _r;
};



#endif // CONSTRAINT_HPP_INCLUDED
