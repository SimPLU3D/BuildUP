#ifndef CONDITION_HPP_INCLUDED
#define CONDITION_HPP_INCLUDED

#include "Expression.hpp"

class Condition
{
public:
    inline virtual ~Condition(){}
    virtual bool predicate(VarValue&) = 0;
};

class AtomCondition: public Condition
{ //simple condition to test if var's value == _value
    Var _var;
    double _value;

public:
    AtomCondition(Var var,double value):_var(var),_value(value){}

    inline int eq(double a,double b,double epsilon = 0.001)
    {
        return std::abs(a-b)<epsilon;
    }

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
