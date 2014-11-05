#ifndef RULE_HPP
#define RULE_HPP

#include "Constraint.hpp"
#include "Energy.hpp"

/***************Rules configured into geometric parameters***************/

class  RuleGeom
{
    double _hMin;
    double _hMax;
    double _hFloor;
    double _lMin;
    double _lMax;
    double _wMin;
    double _wMax;
    std::vector< double > _wPeaks;
    //description strings
    const char* _strH;
    const char* _strL;
    const char* _strW;

public:
    inline RuleGeom(void):_hMin(-1),_hMax(-1),_hFloor(-1),_lMin(-1),_lMax(-1),_wMin(-1),_wMax(-1){}

//sets
    inline void hMin(double h){_hMin=h;}
    inline void hMax(double h){_hMax=h;}
    inline void hFloor(double h){_hFloor=h;}
    inline void lMin(double l){_lMin=l;}
    inline void lMax(double l){_lMax=l;}
    inline void wMin(double w){_wMin=w;}
    inline void wMax(double w){_wMax=w;}
    inline void add_widthPeak(double p){_wPeaks.push_back(p);}
    inline void strH(const char* s){_strH = s;}
    inline void strL(const char* s){_strL = s;}
    inline void strW(const char* s){_strW = s;}

//gets
    inline double hMin() const{return _hMin;}
    inline double hMax() const{return _hMax;}
    inline double hFloor() const{return _hFloor;}
    inline double lMin() const{return _lMin;}
    inline double lMax() const{return _lMax;}
    inline double wMin() const{return _wMin;}
    inline double wMax() const{return _wMax;}
    inline std::vector< double >& wPeaks(){return _wPeaks;}
    inline const char* strH() const{return _strH;}
    inline const char* strL() const{return _strL;}
    inline const char* strW() const{return _strW;}
};

/***************Rules expressed by energy function *****************/
enum class RuleType
{
    DistFront,
    DistSide,
    DistBack,
    DistCorner,
    DistPair,
    HeightDiff,
    LCR,//lot coverage ratio
    FAR //floor area ratio
};

class RuleEnergy
{
// support 1 simple or composite constraint
public:
    inline RuleEnergy(RuleType t,const char* str, Constraint* cs,double aScale,EnergyFuncType aType,double pScale,EnergyFuncType pType)
        :_type(t),_string(str),_constraint(cs),_acceptScale(aScale),_acceptType(aType),_penaltyScale(pScale),_penaltyType(pType) {}
    inline virtual ~RuleEnergy(){if(_constraint!=NULL) delete _constraint;}

    inline RuleType ruleType() const {return _type;}
    inline const char* ruleString() const {return _string;}

    //varValue: values of constrained variables (leftvalue)
    //xExpr: value of the independent variable (only support 0 or 1) in constraint expression (rightvalue)
    typedef std::map<Var,double> VarValue;
    virtual double energy(VarValue varValue,double xExpr=0) = 0;
    virtual bool isValid(VarValue varValue,double xExpr=0) =0;

protected:
    RuleType _type;
    const char* _string;
    Constraint* _constraint;
    double _acceptScale; //acceptance energy function scale
    EnergyFuncType _acceptType; // acceptance energy function type (eg. zero, squared for optimization)
    double _penaltyScale; //penalty energy function scale
    EnergyFuncType _penaltyType; //penalty energy function type (eg. error function)

};


class RuleDynamic : public RuleEnergy
{
//there is one independent variable in rightvalue expression of the constraint
//energy function cannot be created when creating the rule
public:
    inline RuleDynamic(RuleType t,const char* str,Constraint* cs,double aScale,EnergyFuncType aType,double pScale,EnergyFuncType pType)
        :RuleEnergy(t,str,cs,aScale,aType,pScale,pType) {}

    inline double energy(VarValue varValue, double xExpr)
    {
        EnergyPLU* p = _constraint->toEnergy(xExpr,_acceptScale,_acceptType,_penaltyScale,_penaltyType);
        double out = (*p)(varValue);
        delete p;
        return out;
    }

    inline bool isValid(VarValue varValue,double xExpr)
    {
        EnergyPLU* p = _constraint->toEnergy(xExpr,_acceptScale,_acceptType,_penaltyScale,_penaltyType);
        bool out = p->isValid(varValue);
        delete p;
        return out;
    }
};


class RuleStatic : public RuleEnergy
{
//no independent variable in rightvalue expression of the constraint
//energy function can be created when creating the rule
    EnergyPLU* _energy;

public:
    inline RuleStatic(RuleType t,const char* str,Constraint* cs,double aScale,EnergyFuncType aType,double pScale,EnergyFuncType pType)
        :RuleEnergy(t,str,cs,aScale,aType,pScale,pType)
    {_energy = _constraint->toEnergy(0,_acceptScale,_acceptType,_penaltyScale,_penaltyType);}

    inline ~RuleStatic(){if (_energy!=NULL) delete _energy;}

    inline double energy(VarValue varValue,double xExpr=0){return (*_energy)(varValue);}
    inline bool isValid(VarValue varValue,double xExpr=0){return _energy->isValid(varValue);}
};
#endif // RULE_HPP
