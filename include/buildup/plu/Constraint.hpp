#ifndef CONSTRAINT_HPP_INCLUDED
#define CONSTRAINT_HPP_INCLUDED

#ifndef ICL_USE_BOOST_MOVE_IMPLEMENTATION
#define ICL_USE_BOOST_MOVE_IMPLEMENTATION
#endif // ICL_USE_BOOST_MOVE_IMPLEMENTATION

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>

#include "Expression.hpp"
#include "Energy.hpp"

/**
 * @class
 * The Constraint class is a pure abstract class representing the constraint in PLU rules
 * a constraint can involve more than one variables eg. dSide1>2 && dSide2>3
 * a constraint on the same variable can be interpreted by intervals
 * eg. dSide1>2 --> rejection interval (-inf,2] and acception interval (2,+inf)
 * each interval is associated with an energy function
 */
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


/**
 * @class
 * The AtomConstraint class template represents the atomic constraint in PLU rules
 * an atomic constraint involves only one variable and one binary relation
 * eg. dFront>2 is an atomic constraint
 * eg. (2<dFront<4), (dFront1 =0 && dFront2=0) are not atomic constraints
 */
template<typename E>
class AtomConstraint:public Constraint
{
    Var _var;
    Relation _r;
    E _expr;

public:
    inline AtomConstraint(Var v,Relation r,E expr):_var(v),_r(r),_expr(expr) {}

    /**
     *
     * interface function for computing energy based on an atomic constraint
     *
     * @param
     * v: variable and value
     * acceptScale, acceptType: parameters for energy function assigned to acception intervals
     * penaltyScale, penaltyType: parameters for energy function assigned to rejection intervals
     *
     * @example
     * the constraint is dFront>2
     * acceptScale is 1.0, acceptType is EnergyFuncType::Zero
     * penaltyScale is 0.2, penaltyType is EnergyFuncType::Erf (Gaussian error function)
     * therefore, the acception interval is (2,inf) associated with energy function y=0
     * the rejection interval is (-inf,2] associated with energy function y=erf(0.2*(x-2))
     * if input v contains <"dFront", 3>, then energy = 0
     * if input v contains <"dFront", 1.5>, then energy = abs(erf(0.2*(1.5-2)))
     */
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

/**
 * @class
 * The CompConstraint class represents the composite constraint in PLU rules
 * a composite constraint involves more than one atomic constraints connected by AND/OR relation
 * eg. (2<dFront<4), (dFront1 =0 && dFront2=0) are both composite constraints
 * but (2<dFront<4) has to be expressed as (dFront>2 && dFront<4)
 */
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
    /**
     * interface function for computing energy based on a composite constraint
     *
     * @param
     * v: variable and value
     * acceptScale, acceptType: parameters for energy function assigned to acception intervals
     * penaltyScale, penaltyType: parameters for energy function assigned to rejection intervals
     *
     * @example
     * if the constraint involves the same variable eg. (dFront>2 && dFront<4)
     * the intervals can be merged, and energy computation is the same as for atomic constraint
     * if the constraint involves different variable, the energy computation depends on their relation
     * eg. (dFront1 =0 && dFront2=0) the total energy is max(energy(<"dFront1", x>), energy(<"dFront2", x>))
     * eg. (dFront1 =0 || dFront2=0) the total energy is min(energy(<"dFront1", x>), energy(<"dFront2", x>))
     */
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
