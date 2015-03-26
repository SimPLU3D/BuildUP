#ifndef CONDITION_HPP_INCLUDED
#define CONDITION_HPP_INCLUDED

#include "Expression.hpp"

/**
 * @class
 * The Condition class is a pure abstract class representing the condition in PLU rules
 * A condition is the expression in the if clause in a PLU rule
 */
class Condition
{
public:
    inline virtual ~Condition(){}
    virtual bool predicate(VarValue&) = 0;
};

/**
 * @class
 * The AtomCondition class is derived from Condition class representing the atomic condition.
 * An atomic condition involves only one binary relation of one variable.
 * However, the current implementation only support "is equal to" condition
 * eg. if(hasWindow) dPair>=6, the condition is "hasWindow == 1"
 */
class AtomCondition: public Condition
{
    Var _var;
    double _value;

public:
    AtomCondition(Var var,double value):_var(var),_value(value){}

    inline int eq(double a,double b,double epsilon = 0.001)
    {
        return std::abs(a-b)<epsilon;
    }

    /**
     * @function
     * this function is to examine if the condition is statisfied
     * input varValue is of type: std::map<Var,double>
     * there could be many variables stored in the input, so the matched variable has to be found first
     */
    inline bool predicate(VarValue& varValue)
    {
        VarValue::iterator it = varValue.find(_var);
        if(it == varValue.end())
        {
            std::cerr<<"error no matched variable for the condition "<<_var._name<<"\n";
            exit(1);
        }

        if(eq(it->second,_value))
            return true;
        return false;
    }

};

/**
 * @class
 * The CompCondition class is derived from Condition class representing the composite condition.
 * An composite condition is composed of more than one atomic conditions connected by AND/OR relation
 * eg. if(!hasWindowLow && !hasWindowHigh) dPair>hHigh/2
 */
class CompCondition: public Condition
{
    Condition* _leftChild;
    Condition* _rightChild;
    Relation _r;
public:
    inline CompCondition(Condition* lc, Condition* rc,Relation r)
    :_leftChild(lc),_rightChild(rc),_r(r) {}

    inline ~CompCondition()
    {
        if(_leftChild!=NULL) delete _leftChild;
        if(_rightChild!=NULL) delete _rightChild;
    }

    inline bool predicate(VarValue& varValue)
    {
        switch(_r)
        {
        case Relation::And:
            return _leftChild->predicate(varValue) && _rightChild->predicate(varValue);
        case Relation::Or:
            return _leftChild->predicate(varValue) || _rightChild->predicate(varValue);
        default:
            std::cerr<<"error: wrong condition logic";
            exit(1);
        }
    }
};
#endif // CONDITION_HPP_INCLUDED
