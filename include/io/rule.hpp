#ifndef RULE_HPP_INCLUDED
#define RULE_HPP_INCLUDED

#include "plu/Rule.hpp"
#include "plu/Lot.hpp"
namespace io
{
//    Rule* makeRule_overlap()
//    {
//        typedef Expression<Constant> Literal;
//        typedef AtomConstraint<Literal> AC;
//
//        Literal l(Constant(0.0));
//        Constraint* c = new AC(Var("overlap"),RL_Equal,l);
//
//        std::vector<Constraint*> cs;
//        cs.push_back(c);
//
//        Rule* rule = new RuleStatic(RT_Overlap,cs);
//        return rule;
//    }


    Rule* makeRule_dFront(int iTest)
    {
        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;

        switch (iTest){
        case 1:
            {
            Literal l(Constant(3.0));
            Constraint* c = new AC(Var("dFront"),RL_Greater,l);

            std::vector<Constraint*> cs;
            cs.push_back(c);

            return new RuleStatic(RT_DistanceFront,cs);
            }

        case 2:
            {
            Literal l(Constant(0.0));
            Constraint* c1 = new AC(Var("dFront1"),RL_Equal,l);
            Constraint* c2 = new AC(Var("dFront2"),RL_Equal,l);
            Constraint* c12 = new CompConstraint(c1,c2,RL_Or);

            std::vector<Constraint*> cs;
            cs.push_back(c12);

            return new RuleStatic(RT_DistanceFront,cs);
            }

        }

    }

    Rule* makeRule_dSide(int iTest)
    {


        switch (iTest){
        case 1:
            {
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


            Constraint* c1 = new AC1(Var("dSide1"),RL_Equal,l1); //d1=0
            Constraint* c2 = new AC1(Var("dSide2"),RL_Equal,l1); //d2=0
//            Constraint* c1 = new AC1(Var("dSide1"),RL_Equal,l1_); //d1<=1.0
//            Constraint* c2 = new AC1(Var("dSide2"),RL_Equal,l1_); //d2<=1.0

            Constraint* c12 = new CompConstraint(c1,c2,RL_And); //d1=0 && d2=0

            Constraint* c3 = new AC1(Var("dSide1"),RL_Equal,l1); //d1=0
            Constraint* c4 = new AC2(Var("dSide2"),RL_GreaterEqual,vlMax);//d2>=max(6,h)
            Constraint* c34= new CompConstraint(c3,c4,RL_And); //d1=0 && d2>=max(6,h)

            Constraint* c5 = new AC2(Var("dSide1"),RL_GreaterEqual,vlMax);//d1>=max(6,h)
            Constraint* c6 = new AC1(Var("dSide2"),RL_Equal,l1); //d2=0
            Constraint* c56 = new CompConstraint(c5,c6,RL_And); //d1>=max(6,h) && d2=0

            Constraint* c12_34 = new CompConstraint(c12,c34,RL_Or); // d1=0 && d2=0 || d1=0 && d2>=max(6,h)
            Constraint* c12_34_56 = new CompConstraint(c12_34,c56,RL_Or);//d1=0 && d2=0 || d1=0 && d2>=max(6,h) || d1>=max(6,h) && d2=0


            Constraint* c7 = new AC1(Var("dSide1"),RL_Equal,l1); //d1=0
            Constraint* c8 = new AC1(Var("dSide2"),RL_Equal,l1); //d2=0
//            Constraint* c7 = new AC1(Var("dSide1"),RL_Equal,l1_); //d1<=1.0
//            Constraint* c8 = new AC1(Var("dSide2"),RL_Equal,l1_); //d2<=1.0

            Constraint* c78 = new CompConstraint(c7,c8,RL_And); //d1=0 && d2=0

            Constraint* c9 = new AC2(Var("dSide1"),RL_GreaterEqual,vlMax);//d1>=max(6,h)
            Constraint* c10 = new AC2(Var("dSide2"),RL_GreaterEqual,vlMax);//d2>=max(6,h)
            Constraint* c910 = new CompConstraint(c9,c10,RL_And); //d1>=max(6,h) && d2>=max(6,h)

            Constraint* c78_910 = new CompConstraint(c78,c910,RL_Or);//d1=0 && d2=0 || d1>=max(6,h) && d2>=max(6,h)


            std::vector<Constraint*> cs;
            cs.push_back(c12_34_56);
            cs.push_back(c78_910);

            return new RuleDynamic(RT_DistanceSide,cs);

            }

        case 2:
            {
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

            Constraint* c1 = new AC(Var("dSide1"),RL_GreaterEqual,expr);
            Constraint* c2 = new AC(Var("dSide2"),RL_GreaterEqual,expr);
            Constraint* c12 = new CompConstraint(c1,c2,RL_And);

            std::vector<Constraint*> cs;
            cs.push_back(c12);
            return new RuleDynamic(RT_DistanceSide,cs);
            }

        }
    }

    Rule* makeRule_dBack(int iTest)
    {
        switch (iTest){
        case 1:
            {
            typedef Expression<Constant> Literal;
            typedef AtomConstraint<Literal> AC;
            Literal l(Constant(4.0));
            Constraint* c = new AC(Var("dBack"),RL_GreaterEqual,l);

            std::vector<Constraint*> cs;
            cs.push_back(c);

            return new RuleStatic(RT_DistanceBack,cs);
            }

        case 2:
            return NULL;
        }
    }

    Rule* makeRule_dPair(int iTest)
    {
        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;
        typedef AtomConstraint<Literal> AC;

        switch (iTest){
        case 1:
            {
            Literal l1(Constant(6.0));
            Literal l2(Constant(4.0));
            Constraint* c1 = new AC(Var("dPair"),RL_GreaterEqual,l1);
            Constraint* c2 = new AC(Var("dPair"),RL_GreaterEqual,l2);
            Constraint* c12 = new CompConstraint(c1,c2,RL_Or);

            std::vector<Constraint*> cs;
            cs.push_back(c12);

            return new RuleStatic(RT_DistancePairwise,cs);
            }
        case 2:
            {
            typedef BinaryExpression<Variable,Literal,Multiply> BE1;
            typedef BinaryExpression<Variable,Literal,Add> BE2;
            typedef BinaryExpression<BE2,Literal,Max> BE3;
            typedef Expression<BE1> Expr1;
            typedef Expression<BE3> Expr2;

            typedef AtomConstraint<Expr1> AC1;
            typedef AtomConstraint<Expr2> AC2;

            Variable vH1(Var("heightMax"));
            Variable vH2(Var("heightMin"));
            Literal l1(Constant(0.5));
            Literal l2(Constant(-3.0));
            Literal l3(Constant(8.0));
            Expr1 expr1(BE1(vH1,l1));
            Expr2 expr2(BE3(BE2(vH1,l2),l3));
            Expr2 expr3(BE3(BE2(vH2,l2),l3));

            Constraint* c1 = new AC1(Var("dPair"),RL_Greater,expr1);
            Constraint* c2 = new AC2(Var("dPair"),RL_Greater,expr2);
            Constraint* c3 = new AC2(Var("dPair"),RL_Greater,expr3);

            Constraint* c12 = new CompConstraint(c1,c2,RL_Or);
            Constraint* c123 = new CompConstraint(c12,c3,RL_Or);

            std::vector<Constraint*> cs;
            cs.push_back(c123);

            return new RuleDynamic(RT_DistancePairwise,cs);
            }
        }

    }

    Rule* makeRule_ces(Lot* lot, int iTest)
    {

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Constraint* c;

        switch (iTest){
        case 1:
            c = new AC(Var("ces"),RL_LessEqual,Literal(Constant(0.6)));
            lot->_rule._cesMax = 0.6;
            break;
        case 2:
            c = new AC(Var("ces"),RL_LessEqual,Literal(Constant(1.0)));
            lot->_rule._cesMax = 1.0;
            break;
        }

        std::vector<Constraint*> cs;
        cs.push_back(c);

        return new RuleStatic(RT_CES,cs);
     }


    Rule* makeRule_cos(Lot* lot, int iTest)
    {

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Constraint* c;

        switch (iTest){
        case 1:
            c = new AC(Var("cos"),RL_LessEqual,Literal(Constant(3.0)));
            lot->_rule._cosMax = 3.0;
            break;
        case 2:
            break;
        }

        std::vector<Constraint*> cs;
        cs.push_back(c);

        return new RuleStatic(RT_COS,cs);
     }

     void makeRule_height(Lot* lot, int iTest)
     {
        switch (iTest){
        case 1:
            lot->_rule._hMin = 6.0;
            lot->_rule._hMax = 18.0;
            lot->_rule._hFloor = 3.0;
            break;
        case 2:
            lot->_rule._hMin = 3.2;
            lot->_rule._hMax = 24.0;
            lot->_rule._hFloor = 3.0;
            break;
        }
     }

}//namespace io

#endif // RULE_HPP_INCLUDED
