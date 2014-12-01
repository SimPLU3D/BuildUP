#ifndef ENERGY_HPP_INCLUDED
#define ENERGY_HPP_INCLUDED

#ifndef ICL_USE_BOOST_MOVE_IMPLEMENTATION
#define ICL_USE_BOOST_MOVE_IMPLEMENTATION
#endif // ICL_USE_BOOST_MOVE_IMPLEMENTATION

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>
#include "Expression.hpp"
#include <cmath>
#include <map>
#include <vector>
#include <iostream>

enum class EnergyFuncType
{
    Zero,
    Erf,
    Linear,
    Square
};

class EnergyFunc
{
    double _offset;
    double _scale;
    EnergyFuncType _funcType;

public:
    EnergyFunc(double offset,double scale, EnergyFuncType type):_offset(offset),_scale(scale),_funcType(type) {}

    inline double operator() (double x){
        switch (_funcType){
        case EnergyFuncType::Zero:
            return 0;
        case EnergyFuncType::Erf:
            return std::abs(std::erf(_scale*(x-_offset)));
        case EnergyFuncType::Linear:
            return std::abs(_scale*(x-_offset));
        case EnergyFuncType::Square:
            return _scale*(x-_offset)*(x-_offset);
        default:
            std::cerr<<"undefined function type\n";
            exit(1);
        }
    }
};

class EnergyPLU
{
public:
    EnergyPLU() {}
    virtual ~EnergyPLU() {}

    typedef std::map<Var,double> VarMap;
    virtual double operator()(VarMap&) = 0 ;
    virtual bool isValid(VarMap&) = 0;
};

class EnergyComposite: public EnergyPLU
{
    EnergyPLU* _leftChild;
    EnergyPLU* _rightChild;
    Relation _r;

public:
    EnergyComposite(EnergyPLU* lc,EnergyPLU* rc,Relation r);
    ~EnergyComposite();

    double operator()(VarMap&);
    bool isValid(VarMap&);
};


class EnergyPiecewise : public EnergyPLU
{
    typedef boost::icl::continuous_interval<double> Interval;
    typedef std::vector<Interval> Intervals;
    typedef boost::icl::interval_map<double,int> IntervalMap;
    typedef std::map<int,EnergyFunc*> EnergyMap;

    Var _var;
    IntervalMap _intervalMap; //interval - index (key of _energyMap)
    EnergyMap _energyMap; //index - Energy*

    void generateEnergyMap(Intervals&,double,EnergyFuncType,double,EnergyFuncType);

public:
    EnergyPiecewise(Var,Intervals,double,EnergyFuncType,double,EnergyFuncType);
    ~EnergyPiecewise();

    double operator() (VarMap&);
    bool isValid(VarMap&);
};


#endif // ENERGY_HPP_INCLUDED
