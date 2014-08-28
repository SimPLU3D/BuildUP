#ifndef MAKERULE_HPP_INCLUDED
#define MAKERULE_HPP_INCLUDED

#include "buildup/plu/Rule.hpp"

RuleEnergy* makeRule_dFront()
{
    typedef Expression<Constant> Literal;
    typedef AtomConstraint<Literal> AC;

    const char* strRule = "dFront>3";

    Literal l(Constant(3.0));
    Constraint* c = new AC(Var("dFront"),Relation::Greater,l);

    return new RuleStatic(RuleType::DistFront,strRule, c
                          ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
}

RuleEnergy* makeRule_dSide()
{
    const char* strRule = "dSide1=0 && dSide2=0 || dSide1=0 && dSide2>=max(6,h) || dSide1>=max(6,h) && dSide2=0";

    typedef Expression<Constant> Literal;
    typedef Expression<Var> Variable;
    typedef BinaryExpression<Variable,Literal,Max> BEMax;
    typedef Expression<BEMax> VarLitMax;
    typedef AtomConstraint<Literal> AC1;
    typedef AtomConstraint<VarLitMax> AC2;

    Literal l1(Constant(0.0));
    Literal l1_(Constant(1.0));
    Literal l2(Constant(6.0));
    Variable varH(Var("height"));
    VarLitMax vlMax(BEMax(varH,l2));


    Constraint* c1 = new AC1(Var("dSide1"),Relation::Equal,l1); //d1=0
    Constraint* c2 = new AC1(Var("dSide2"),Relation::Equal,l1); //d2=0
    Constraint* c12 = new CompConstraint(c1,c2,Relation::And); //d1=0 && d2=0

    Constraint* c3 = new AC1(Var("dSide1"),Relation::Equal,l1); //d1=0
    Constraint* c4 = new AC2(Var("dSide2"),Relation::GreaterEqual,vlMax);//d2>=max(6,h)
    Constraint* c34= new CompConstraint(c3,c4,Relation::And); //d1=0 && d2>=max(6,h)

    Constraint* c5 = new AC2(Var("dSide1"),Relation::GreaterEqual,vlMax);//d1>=max(6,h)
    Constraint* c6 = new AC1(Var("dSide2"),Relation::Equal,l1); //d2=0
    Constraint* c56 = new CompConstraint(c5,c6,Relation::And); //d1>=max(6,h) && d2=0

    Constraint* c12_34 = new CompConstraint(c12,c34,Relation::Or); // d1=0 && d2=0 || d1=0 && d2>=max(6,h)
    Constraint* c12_34_56 = new CompConstraint(c12_34,c56,Relation::Or);//d1=0 && d2=0 || d1=0 && d2>=max(6,h) || d1>=max(6,h) && d2=0

    return new RuleDynamic(RuleType::DistSide,strRule,c12_34_56
                           ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
}

RuleEnergy* makeRule_dBack()
{
    const char* strRule = "dBack>=4";

    typedef Expression<Constant> Literal;
    typedef AtomConstraint<Literal> AC;
    Literal l(Constant(4.0));
    Constraint* c = new AC(Var("dBack"),Relation::GreaterEqual,l);

    return new RuleStatic(RuleType::DistBack,strRule,c
                          ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
}


RuleEnergy* makeRule_dPair()
{
    typedef Expression<Constant> Literal;
    typedef Expression<Var> Variable;
    typedef AtomConstraint<Literal> AC;

    const char* strRule = "dPair>=6";

    Literal l(Constant(6.0));
    Constraint* c = new AC(Var("dPair"),Relation::GreaterEqual,l);

    return new RuleStatic(RuleType::DistPair,strRule,c
                          ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
}


RuleEnergy* makeRule_hDiff()
{
    const char* strRule = "hDiff<=6";

    typedef Expression<Constant> Literal;
    typedef AtomConstraint<Literal> AC;
    Constraint* c = new AC(Var("hDiff"),Relation::LessEqual,Literal(Constant(6.0)));

    return new RuleStatic(RuleType::HeightDiff,strRule,c
                          ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
}


RuleEnergy* makeRule_lcr()
{
    const char* strRule = "lcr<=0.6";

    typedef Expression<Constant> Literal;
    typedef AtomConstraint<Literal> AC;
    Constraint* c = new AC(Var("lcr"),Relation::LessEqual,Literal(Constant(0.6)));

    return new RuleStatic(RuleType::LCR,strRule,c
                          ,1.0,EnergyFuncType::Zero,2,EnergyFuncType::Erf);
}


RuleEnergy* makeRule_far()
{
    const char* strRule = "far<=4";

    typedef Expression<Constant> Literal;
    typedef AtomConstraint<Literal> AC;
    Constraint* c = new AC(Var("far"),Relation::LessEqual,Literal(Constant(4.0)));

    return new RuleStatic(RuleType::FAR,strRule,c
                          ,0.1,EnergyFuncType::Square,1,EnergyFuncType::Erf);
}

RuleGeom* makeRuleGeom()
{
    RuleGeom* rule = new RuleGeom();

    rule->hMin(6);
    rule->hMax(18);
    rule->hFloor(3);
    rule->strH("[6,18]");
    rule->rho(std::vector<double> {2.5,5}) ;

    return rule;
}

#endif // MAKERULE_HPP_INCLUDED
