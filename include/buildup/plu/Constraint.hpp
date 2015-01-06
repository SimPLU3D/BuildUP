#ifndef CONSTRAINT_HPP_INCLUDED
#define CONSTRAINT_HPP_INCLUDED

#ifndef ICL_USE_BOOST_MOVE_IMPLEMENTATION
#define ICL_USE_BOOST_MOVE_IMPLEMENTATION
#endif // ICL_USE_BOOST_MOVE_IMPLEMENTATION

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>

#include "Expression.hpp"
#include "Energy.hpp"


class Constraint
{
public:
    inline virtual ~Constraint(){};

    typedef boost::icl::continuous_interval<double> Interval;
    typedef std::vector<Interval> Intervals;
    typedef boost::icl::interval_map<double,int> IntervalMap;
    typedef std::numeric_limits<double> Limit;

    virtual Var getVar(void)=0;
    virtual bool isForVar(Var)=0;
    virtual bool isForSameVar(void)=0;
    virtual Intervals toInterval(VarValue&)=0;
    virtual EnergyPLU* toEnergy(VarValue&,double acceptScale, EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType)=0;
};

template<typename E> //E is an expression type
class AtomConstraint:public Constraint
{
    Var _var;
    Relation _r;
    E _expr;

public:
    inline AtomConstraint(Var v,Relation r,E expr):_var(v),_r(r),_expr(expr) {}

    inline EnergyPLU* toEnergy(VarValue& v,double acceptScale, EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType)
    {return new EnergyPiecewise(_var,toInterval(v),acceptScale,acceptType,penaltyScale,penaltyType);}

private:
    inline Var getVar(void){return _var;}
    inline bool isForVar(Var v){return _var==v;}
    inline bool isForSameVar(void){return true;}

    inline Intervals toInterval(VarValue& v)
    {
        Intervals intervals;
        double y = _expr(v);
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

};




class CompConstraint: public Constraint
{
    Constraint* _leftChild;
    Constraint* _rightChild;
    Relation _r;

public:
    inline CompConstraint(Constraint* lc, Constraint* rc,Relation r)
    :_leftChild(lc),_rightChild(rc),_r(r) {}

    inline ~CompConstraint()
    {
        if(_leftChild!=NULL) delete _leftChild;
        if(_rightChild!=NULL) delete _rightChild;
    }

    EnergyPLU* toEnergy(VarValue& v,double acceptScale,EnergyFuncType acceptType,double penaltyScale,EnergyFuncType penaltyType);

private:
    inline Var getVar(void){return _leftChild->getVar(); }  //leftest var of its children
    inline bool isForVar(Var v){return _rightChild->isForVar(v) && _leftChild->isForVar(v);}
    inline bool isForSameVar(void){return _leftChild->isForVar(_leftChild->getVar()) && _rightChild->isForVar(_leftChild->getVar());}

    Intervals toInterval(VarValue&);
    Intervals intervalOR(Intervals,Intervals);
    Intervals intervalAND(Intervals,Intervals);
};


#endif // CONSTRAINT_HPP_INCLUDED
