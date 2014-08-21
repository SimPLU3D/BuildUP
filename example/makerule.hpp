#ifndef RULE_HPP_INCLUDED
#define RULE_HPP_INCLUDED

#include "Rule.hpp"
#include "Lot.hpp"
namespace io
{
    RuleEnergy* makeRule_dFront(int iTest)
    {
        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;

        switch (iTest){
        case 1:
            {
            const char* strRule = "dFront>3";

            Literal l(Constant(3.0));
            Constraint* c = new AC(Var("dFront"),Relation::Greater,l);

            return new RuleStatic(RuleType::DistFront,strRule, c
            ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        case 2:
            {
            const char* strRule = "dFront1=0 || dFront1>2.5 && dFront2=0 || dFront2>2.5";

            Literal l1(Constant(0.0));
            Literal l2(Constant(2.5));
            Constraint* c1 = new AC(Var("dFront1"),Relation::Equal,l1);
            Constraint* c2 = new AC(Var("dFront1"),Relation::Greater,l2);
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or);

            Constraint* c3 = new AC(Var("dFront2"),Relation::Equal,l1);
            Constraint* c4 = new AC(Var("dFront2"),Relation::Greater,l2);
            Constraint* c34 = new CompConstraint(c3,c4,Relation::Or);

            Constraint* c12_34 = new CompConstraint(c12,c34,Relation::And);

            return new RuleStatic(RuleType::DistFront,strRule,c12_34
            ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        case 4:
            {
            const char* strRule = "dFront1=0 || dFront2=0 || dFront3=0";

            Literal l(Constant(0.0));
            Constraint* c1 = new AC(Var("dFront1"),Relation::Equal,l);
            Constraint* c2 = new AC(Var("dFront2"),Relation::Equal,l);
            Constraint* c3 = new AC(Var("dFront3"),Relation::Equal,l);
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or);
            Constraint* c123 = new CompConstraint(c12,c3,Relation::Or);

            return new RuleStatic(RuleType::DistFront,strRule,c123
            ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        default:
            return 0;

        }

    }

    RuleEnergy* makeRule_dSide(int iTest)
    {


        switch (iTest){
        case 1:
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
//            Constraint* c1 = new AC1(Var("dSide1"),Relation::Equal,l1_); //d1<=1.0
//            Constraint* c2 = new AC1(Var("dSide2"),Relation::Equal,l1_); //d2<=1.0

            Constraint* c12 = new CompConstraint(c1,c2,Relation::And); //d1=0 && d2=0

            Constraint* c3 = new AC1(Var("dSide1"),Relation::Equal,l1); //d1=0
            Constraint* c4 = new AC2(Var("dSide2"),Relation::GreaterEqual,vlMax);//d2>=max(6,h)
            Constraint* c34= new CompConstraint(c3,c4,Relation::And); //d1=0 && d2>=max(6,h)

            Constraint* c5 = new AC2(Var("dSide1"),Relation::GreaterEqual,vlMax);//d1>=max(6,h)
            Constraint* c6 = new AC1(Var("dSide2"),Relation::Equal,l1); //d2=0
            Constraint* c56 = new CompConstraint(c5,c6,Relation::And); //d1>=max(6,h) && d2=0

            Constraint* c12_34 = new CompConstraint(c12,c34,Relation::Or); // d1=0 && d2=0 || d1=0 && d2>=max(6,h)
            Constraint* c12_34_56 = new CompConstraint(c12_34,c56,Relation::Or);//d1=0 && d2=0 || d1=0 && d2>=max(6,h) || d1>=max(6,h) && d2=0


//            Constraint* c7 = new AC1(Var("dSide1"),Relation::Equal,l1); //d1=0
//            Constraint* c8 = new AC1(Var("dSide2"),Relation::Equal,l1); //d2=0
//            Constraint* c78 = new CompConstraint(c7,c8,Relation::And); //d1=0 && d2=0
//
//            Constraint* c9 = new AC2(Var("dSide1"),Relation::GreaterEqual,vlMax);//d1>=max(6,h)
//            Constraint* c10 = new AC2(Var("dSide2"),Relation::GreaterEqual,vlMax);//d2>=max(6,h)
//            Constraint* c910 = new CompConstraint(c9,c10,Relation::And); //d1>=max(6,h) && d2>=max(6,h)
//
//            Constraint* c78_910 = new CompConstraint(c78,c910,Relation::Or);//d1=0 && d2=0 || d1>=max(6,h) && d2>=max(6,h)


            //only use first band for now
            return new RuleDynamic(RuleType::DistSide,strRule,c12_34_56
            ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);

            }

        case 2:
            {
            const char* strRule = "dSide1>=max((h-3)/2,3) && dSide2>=max((h-3)/2,3)";

            typedef Expression<Constant> Literal;
            typedef Expression<Var> Variable;
            typedef BinaryExpression<Variable,Literal,Add> BE1;
            typedef BinaryExpression<BE1,Literal,Multiply> BE2;
            typedef BinaryExpression<BE2,Literal,Max> BE3;
            typedef Expression<BE3> Expr;
            typedef AtomConstraint<Expr> AC;

            Variable varH(Var("height"));
            Literal l1(Constant(-3.0));
            Literal l2(Constant(0.5));
            Literal l3(Constant(3.0));
            Expr expr(BE3(BE2(BE1(varH,l1),l2),l3)); //max((h-3)/2,3)

            Constraint* c1 = new AC(Var("dSide1"),Relation::GreaterEqual,expr);
            Constraint* c2 = new AC(Var("dSide2"),Relation::GreaterEqual,expr);
            Constraint* c12 = new CompConstraint(c1,c2,Relation::And);

            return new RuleDynamic(RuleType::DistSide,strRule,c12
            ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        case 4:
            {
            const char* strRule = "dSide>=max(4,0.3h) || dSide=0";

            typedef Expression<Constant> Literal;
            typedef Expression<Var> Variable;
            typedef BinaryExpression<Variable,Literal,Multiply> BE1;
            typedef BinaryExpression<BE1,Literal,Max> BE2;
            typedef Expression<BE2> Expr;
            typedef AtomConstraint<Expr> AC1;
            typedef AtomConstraint<Literal> AC2;

            Variable varH(Var("height"));
            Literal l1(Constant(0.3));
            Literal l2(Constant(4.0));
            Expr expr(BE2(BE1(varH,l1),l2)); //max(4,0.3h)
            Literal l3(Constant(0.0));

            Constraint* c1 = new AC1(Var("dSide"),Relation::GreaterEqual,expr);
            Constraint* c2 = new AC2(Var("dSide"),Relation::Equal,l3);
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or);

            return new RuleDynamic(RuleType::DistSide,strRule,c12
            ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        default:
            return 0;
        }
    }

    RuleEnergy* makeRule_dBack(int iTest)
    {
        switch (iTest){
        case 1:
            {
                const char* strRule = "dBack>=4";

                typedef Expression<Constant> Literal;
                typedef AtomConstraint<Literal> AC;
                Literal l(Constant(4.0));
                Constraint* c = new AC(Var("dBack"),Relation::GreaterEqual,l);

                return new RuleStatic(RuleType::DistBack,strRule,c
                ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        default:
            return 0;
        }
    }


    RuleEnergy* makeRule_dPair(int iTest)
    {
        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;
        typedef AtomConstraint<Literal> AC;

        switch (iTest){
        case 1:
            {
                const char* strRule = "dPair>=6";

                Literal l(Constant(6.0));
                Constraint* c = new AC(Var("dPair"),Relation::GreaterEqual,l);

                return new RuleStatic(RuleType::DistPair,strRule,c
                ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }
        case 2:
            {
                const char* strRule = "dPair> hMax/2 && dPair>max(hMax-3,8.0)";

                typedef BinaryExpression<Variable,Literal,Multiply> BE1;
                typedef BinaryExpression<Variable,Literal,Add> BE2;
                typedef BinaryExpression<BE2,Literal,Max> BE3;
                typedef Expression<BE1> Expr1;
                typedef Expression<BE3> Expr2;

                typedef AtomConstraint<Expr1> AC1;
                typedef AtomConstraint<Expr2> AC2;

                Variable vH(Var("heightMax"));
                Literal l1(Constant(0.5));
                Literal l2(Constant(-3.0));
                Literal l3(Constant(8.0));
                Expr1 expr1(BE1(vH,l1));
                Expr2 expr2(BE3(BE2(vH,l2),l3));

                Constraint* c1 = new AC1(Var("dPair"),Relation::Greater,expr1);
                Constraint* c2 = new AC2(Var("dPair"),Relation::Greater,expr2);

                Constraint* c12 = new CompConstraint(c1,c2,Relation::And);

                return new RuleDynamic(RuleType::DistPair,strRule,c12
                ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }
        case 4:
            {
                const char* strRule = "dPair>=max(6,0.3hMax)";

                typedef Expression<Constant> Literal;
                typedef Expression<Var> Variable;
                typedef BinaryExpression<Variable,Literal,Multiply> BE1;
                typedef BinaryExpression<BE1,Literal,Max> BE2;
                typedef Expression<BE2> Expr;
                typedef AtomConstraint<Expr> AC;

                Variable varH(Var("heightMax"));
                Literal l1(Constant(0.3));
                Literal l2(Constant(4.0));
                Expr expr(BE2(BE1(varH,l1),l2)); //max(6,0.3hMax)

                Constraint* c = new AC(Var("dPair"),Relation::GreaterEqual,expr);

                return new RuleDynamic(RuleType::DistPair,strRule,c
                ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
            }

        default:
            return 0;
        }

    }


    RuleEnergy* makeRule_hDiff(int iTest)
    {
        const char* strRule = "hDiff<=6";

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Constraint* c = new AC(Var("hDiff"),Relation::LessEqual,Literal(Constant(6.0)));

        return new RuleStatic(RuleType::HeightDiff,strRule,c
        ,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);
     }


    RuleEnergy* makeRule_lcr(int iTest)
    {

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Constraint* c;
        const char* strRule;

        switch (iTest){
        case 1:
            strRule = "lcr<=0.6";
            c = new AC(Var("lcr"),Relation::LessEqual,Literal(Constant(0.6)));
            break;
        case 2:
            strRule = "lcr<=1";
            c = new AC(Var("lcr"),Relation::LessEqual,Literal(Constant(1.0)));
            break;
        case 4:
            strRule = "lcr<=0.7";
            c = new AC(Var("lcr"),Relation::LessEqual,Literal(Constant(0.7)));
            break;

        default:
            c=0;

        }

        return new RuleStatic(RuleType::LCR,strRule,c
        ,1.0,EnergyFuncType::Zero,2,EnergyFuncType::Erf);
     }


    RuleEnergy* makeRule_far(int iTest)
    {

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Constraint* c;
        const char* strRule;

        switch (iTest){
        case 1:
            strRule = "far<=4";
            c = new AC(Var("far"),Relation::LessEqual,Literal(Constant(4.0)));
            break;
        case 2:
            strRule = "far<=4";
            c = new AC(Var("far"),Relation::LessEqual,Literal(Constant(4.0)));
            break;
        case 4:
            strRule = "far<=4";
            c = new AC(Var("far"),Relation::LessEqual,Literal(Constant(4.0)));
            break;
        }

        return new RuleStatic(RuleType::FAR,strRule,c
        ,0.1,EnergyFuncType::Square,1,EnergyFuncType::Erf);
     }

    RuleGeom* makeRuleGeom(int iTest)
    {
        RuleGeom* rule = new RuleGeom();

        switch (iTest){
        case 1:
            rule->hMin(6);
            rule->hMax(18);
            rule->hFloor(3);
            rule->strH("[6,18]");
            rule->rho(std::vector<double> {2.5,5}) ;
            break;
        case 2:
            rule->hMin(3.2);
            rule->hMax(24);
            rule->hFloor(3);
            rule->strH("[3.2,24]");
            rule->rho(std::vector<double> {2.5,5});
            break;
        case 4:
            rule->hMin(6);
            rule->hMax(24);
            rule->hFloor(3);
            rule->strH("[6,24]");
            rule->rho(std::vector<double> {2.5,5,7.5});
            break;
        }
        return rule;
    }


    bool isRectLike(int iTest)
    {
        switch (iTest){
        case 1:
            return true;
        case 2:
            return false;
        case 4:
            return false;
        default:
            return false;
        }
    }
}//namespace io

#endif // RULE_HPP_INCLUDED
