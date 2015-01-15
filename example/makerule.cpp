
#include "makerule.hpp"

Rule* makeRule_dFront(int iTest)
{
    switch (iTest){
    case 1:
    {
        const char* strRule = "dFront>3";

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;

        Literal l(Constant(3.0));
        Constraint* c = new AC(Var("dFront"),Relation::Greater,l);

        Rule* rule = new Rule(RuleType::DistFront,strRule);
        rule->addRuleDirectly(new RuleStatic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;
    }

    case 2:
    {
        const char* strRule = "(dFront1=0 || dFront1>2.5) && (dFront2=0 || dFront2>2.5)";

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Literal l1(Constant(0.0));
        Literal l2(Constant(2.5));
        Constraint* c1 = new AC(Var("dFront1"),Relation::Equal,l1);
        Constraint* c2 = new AC(Var("dFront1"),Relation::Greater,l2);
        Constraint* c12 = new CompConstraint(c1,c2,Relation::Or);

        Constraint* c3 = new AC(Var("dFront2"),Relation::Equal,l1);
        Constraint* c4 = new AC(Var("dFront2"),Relation::Greater,l2);
        Constraint* c34 = new CompConstraint(c3,c4,Relation::Or);

        Constraint* c12_34 = new CompConstraint(c12,c34,Relation::And);

        Rule* rule = new Rule(RuleType::DistFront,strRule);
        rule->addRuleDirectly(new RuleStatic(c12_34,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;
    }


    case 3:
    {

        const char* strRule = "dFront=0";// || dFront>h";

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;

        Literal l(Constant(0.0));
        Constraint* c = new AC(Var("dFront"),Relation::Equal,l);

        Rule* rule = new Rule(RuleType::DistFront,strRule);
        rule->addRuleDirectly(new RuleStatic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;
    }

    default:
        return 0;

    }

}

Rule* makeRule_dSide(int iTest)
{
    switch (iTest){
    case 1:
    {
        const char* strRule = "\n\
        dSide1=0 && dSide2=0 ||\n\
        dSide1=0 && dSide2>=max(6,h)||\n\
        dSide1>=max(6,h) && dSide2=0";

        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;
        typedef BinaryExpression<Variable,Literal,Max> BEMax;
        typedef Expression<BEMax> VarLitMax;
        typedef AtomConstraint<Literal> AC1;
        typedef AtomConstraint<VarLitMax> AC2;

        Literal l1(Constant(0.0));
        Literal l2(Constant(6.0));
        Variable varH(Var("h"));
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

        Rule* rule = new Rule(RuleType::DistSide,strRule);
        rule->addRuleDirectly(new RuleDynamic(c12_34_56,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;
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

        Variable varH(Var("h"));
        Literal l1(Constant(-3.0));
        Literal l2(Constant(0.5));
        Literal l3(Constant(3.0));
        Expr expr(BE3(BE2(BE1(varH,l1),l2),l3)); //max((h-3)/2,3)

        Constraint* c1 = new AC(Var("dSide1"),Relation::GreaterEqual,expr);
        Constraint* c2 = new AC(Var("dSide2"),Relation::GreaterEqual,expr);
        Constraint* c12 = new CompConstraint(c1,c2,Relation::And);

        Rule* rule = new Rule(RuleType::DistSide,strRule);
        rule->addRuleDirectly(new RuleDynamic(c12,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;
    }

    case 3:
    {
        const char* strRule = "\n\
        if(hasWindowSide1 && hasWindowSide2)\n\
            (dSide1=0||dSide1>max(6,h-3.5)) && (dSide2=0||dSide2>max(6,h-3.5))\n\
        if(hasWindowSide1 && !hasWindowSide2)\n\
            (dSide1=0||dSide1>max(6,h-3.5)) && (dSide2=0||dSide2>max(3,h/2))\n\
        if(!hasWindowSide1 && hasWindowSide2)\n\
            (dSide1=0||dSide1>max(3,h/2)) && (dSide2=0||dSide2>max(6,h-3.5))\n\
        if(!hasWindowSide1 && !hasWindowSide2)\n\
            (dSide1=0||dSide1>max(3,h/2)) && (dSide2=0||dSide2>max(3,h/2))";

        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;

        typedef BinaryExpression<Variable,Literal,Add> BE1;
        typedef BinaryExpression<Literal,BE1,Max> BE2;
        typedef Expression<BE2> Expr1;

        typedef BinaryExpression<Variable,Literal,Multiply> BE3;
        typedef BinaryExpression<Literal,BE3,Max> BE4;
        typedef Expression<BE4> Expr2;

        typedef AtomConstraint<Literal> AC0;
        typedef AtomConstraint<Expr1> AC1;
        typedef AtomConstraint<Expr2> AC2;

        Variable varH(Var("h"));
        Literal l0(Constant(0.0));
        Literal l1(Constant(-3.5));
        Literal l2(Constant(6.0));
        Literal l3(Constant(0.5));
        Literal l4(Constant(3.0));
        Expr1 expr1(BE2(l2,BE1(varH,l1))); //max(6,h-3.5)
        Expr2 expr2(BE4(l4,BE3(varH,l3))); //max(3,h/2)

        Rule* rule = new Rule(RuleType::DistSide,strRule);

        //if(hasWindowSide1 && hasWindowSide2)
        {
            Condition* cd1 = new AtomCondition(Var("hasWindowSide1"),1.);
            Condition* cd2 = new AtomCondition(Var("hasWindowSide2"),1.);
            Condition* cd12 = new CompCondition(cd1,cd2,Relation::And);

            Constraint* c1 = new AC0(Var("dSide1"),Relation::Equal,l0); //dSide1=0
            Constraint* c2 = new AC1(Var("dSide1"),Relation::Greater,expr1); // dSide1>max(6,h-3.5)
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or); //dSide1=0 || dSide1>max(6,h-3.5)

            Constraint* c3 = new AC0(Var("dSide2"),Relation::Equal,l0); //dSide2=0
            Constraint* c4 = new AC1(Var("dSide2"),Relation::Greater,expr1); // dSide2>max(6,h-3.5)
            Constraint* c34 = new CompConstraint(c3,c4,Relation::Or); //dSide2=0 || dSide2>max(6,h-3.5)

            Constraint* c12_34 = new CompConstraint(c12,c34,Relation::And);

            rule->addConditionDirectly(cd12);
            rule->addRuleDirectly(new RuleDynamic(c12_34,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }

        //if(hasWindowSide1 && !hasWindowSide2)
        {
            Condition* cd1 = new AtomCondition(Var("hasWindowSide1"),1.);
            Condition* cd2 = new AtomCondition(Var("hasWindowSide2"),0.);
            Condition* cd12 = new CompCondition(cd1,cd2,Relation::And);

            Constraint* c1 = new AC0(Var("dSide1"),Relation::Equal,l0); //dSide1=0
            Constraint* c2 = new AC1(Var("dSide1"),Relation::Greater,expr1); // dSide1>max(6,h-3.5)
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or); //dSide1=0 || dSide1>max(6,h-3.5)

            Constraint* c3 = new AC0(Var("dSide2"),Relation::Equal,l0); //dSide2=0
            Constraint* c4 = new AC2(Var("dSide2"),Relation::Greater,expr2); // dSide2>max(3,h/2)
            Constraint* c34 = new CompConstraint(c3,c4,Relation::Or); //dSide2=0 || dSide2>max(3,h/2)

            Constraint* c12_34 = new CompConstraint(c12,c34,Relation::And);

            rule->addConditionDirectly(cd12);
            rule->addRuleDirectly(new RuleDynamic(c12_34,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }

        //if(!hasWindowSide1 && hasWindowSide2)
        {
            Condition* cd1 = new AtomCondition(Var("hasWindowSide1"),0.);
            Condition* cd2 = new AtomCondition(Var("hasWindowSide2"),1.);
            Condition* cd12 = new CompCondition(cd1,cd2,Relation::And);

            Constraint* c1 = new AC0(Var("dSide1"),Relation::Equal,l0); //dSide1=0
            Constraint* c2 = new AC2(Var("dSide1"),Relation::Greater,expr2); // dSide1>max(3,h/2)
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or); //dSide1=0 || dSide1>max(3,h/2)

            Constraint* c3 = new AC0(Var("dSide2"),Relation::Equal,l0); //dSide2=0
            Constraint* c4 = new AC1(Var("dSide2"),Relation::Greater,expr1); // dSide2>max(6,h-3.5)
            Constraint* c34 = new CompConstraint(c3,c4,Relation::Or); //dSide2=0 || dSide2>max(6,h-3.5)

            Constraint* c12_34 = new CompConstraint(c12,c34,Relation::And);

            rule->addConditionDirectly(cd12);
            rule->addRuleDirectly(new RuleDynamic(c12_34,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }

        //if(!hasWindowSide1 && !hasWindowSide2)
        {
            Condition* cd1 = new AtomCondition(Var("hasWindowSide1"),0.);
            Condition* cd2 = new AtomCondition(Var("hasWindowSide2"),0.);
            Condition* cd12 = new CompCondition(cd1,cd2,Relation::And);

            Constraint* c1 = new AC0(Var("dSide1"),Relation::Equal,l0); //dSide1=0
            Constraint* c2 = new AC2(Var("dSide1"),Relation::Greater,expr2); // dSide1>max(3,h/2)
            Constraint* c12 = new CompConstraint(c1,c2,Relation::Or); //dSide1=0 || dSide1>max(3,h/2)

            Constraint* c3 = new AC0(Var("dSide2"),Relation::Equal,l0); //dSide2=0
            Constraint* c4 = new AC2(Var("dSide2"),Relation::Greater,expr2); // dSide2>max(3,h/2)
            Constraint* c34 = new CompConstraint(c3,c4,Relation::Or); //dSide2=0 || dSide2>max(3,h/2)

            Constraint* c12_34 = new CompConstraint(c12,c34,Relation::And);

            rule->addConditionDirectly(cd12);
            rule->addRuleDirectly(new RuleDynamic(c12_34,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }

        return rule;
    }

    default:
        return 0;
    }
}

Rule* makeRule_dBack(int iTest)
{
    switch (iTest){
    case 1:
    {
        const char* strRule = "dBack>=4";

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;
        Literal l(Constant(4.0));
        Constraint* c = new AC(Var("dBack"),Relation::GreaterEqual,l);

        Rule* rule = new Rule(RuleType::DistBack,strRule);
        rule->addRuleDirectly(new RuleStatic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;

    }
    case 2: //no back rule
        return 0;

    case 3:
    {
        const char* strRule = "dBack=0 || dBack>h";

        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;
        typedef AtomConstraint<Literal> AC1;
        typedef AtomConstraint<Variable> AC2;

        Literal l(Constant(0.0));
        Variable v(Var("h"));
        Constraint* c1 = new AC1(Var("dBack"),Relation::Equal,l);
        Constraint* c2 = new AC2(Var("dBack"),Relation::Greater,v);
        Constraint* c12 = new CompConstraint(c1,c2,Relation::Or);

        Rule* rule = new Rule(RuleType::DistBack,strRule);
        rule->addRuleDirectly(new RuleDynamic(c12,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;

    }

    default:
        return 0;
    }
}


Rule* makeRule_dPair(int iTest)
{
    switch (iTest){
    case 1:
    {
        const char* strRule = "dPair>=6";

        typedef Expression<Constant> Literal;
        typedef AtomConstraint<Literal> AC;

        Literal l(Constant(6.0));
        Constraint* c = new AC(Var("dPair"),Relation::GreaterEqual,l);

        Rule* rule = new Rule(RuleType::DistPair,strRule);
        rule->addRuleDirectly(new RuleStatic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        return rule;

    }

//    //no window rules
//    case 2:
//    {
//        const char* strRule = "dPair> hMax/2 && dPair>max(hMax-3,8.0)";
//
//        typedef Expression<Constant> Literal;
//        typedef Expression<Var> Variable;
//
//        typedef BinaryExpression<Variable,Literal,Multiply> BE1;
//        typedef BinaryExpression<Variable,Literal,Add> BE2;
//        typedef BinaryExpression<BE2,Literal,Max> BE3;
//        typedef Expression<BE1> Expr1;
//        typedef Expression<BE3> Expr2;
//
//        typedef AtomConstraint<Expr1> AC1;
//        typedef AtomConstraint<Expr2> AC2;
//
//        Variable vH(Var("heightMax"));
//        Literal l1(Constant(0.5));
//        Literal l2(Constant(-3.0));
//        Literal l3(Constant(8.0));
//        Expr1 expr1(BE1(vH,l1));
//        Expr2 expr2(BE3(BE2(vH,l2),l3));
//
//        Constraint* c1 = new AC1(Var("dPair"),Relation::Greater,expr1);
//        Constraint* c2 = new AC2(Var("dPair"),Relation::Greater,expr2);
//
//        Constraint* c12 = new CompConstraint(c1,c2,Relation::And);
//
//        Rule* rule = new Rule(RuleType::DistPair,strRule);
//        rule->addRuleDirectly(new RuleDynamic(c12,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
//        return rule;
//
//    }
    case 2:
    {
        const char* strRule = "\n\
        if(!hasWindowLow && !hasWindowHigh) {dPair>hHigh/2}\n\
        if(!hasWindowLow && hasWindowHigh)  {dPair>max(hLow-3,8)}\n\
        if(hasWindowLow) {dPair>max(hHigh-3,8)}";

        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;

        typedef BinaryExpression<Variable,Literal,Multiply> BE1;
        typedef Expression<BE1> Expr1;

        typedef BinaryExpression<Variable,Literal,Add> VLAdd;
        typedef BinaryExpression<VLAdd,Literal,Max> BE2;
        typedef Expression<BE2> Expr2;

        typedef AtomConstraint<Expr1> AC1;
        typedef AtomConstraint<Expr2> AC2;

        Literal l1(Constant(0.5));
        Literal l2(Constant(-3));
        Literal l3(Constant(8));
        Variable vhLow(Var("hLow"));
        Variable vhHigh(Var("hHigh"));
        Expr1 expr1(BE1(vhHigh,l1));
        Expr2 expr2(BE2(VLAdd(vhLow,l2),l3));
        Expr2 expr3(BE2(VLAdd(vhHigh,l2),l3));

        Rule* rule = new Rule(RuleType::DistSide,strRule);
        //if(!hasWindowLow && !hasWindowHigh)
        {
            Condition* cd1 = new AtomCondition(Var("hasWindowLow"),0.);
            Condition* cd2 = new AtomCondition(Var("hasWindowHigh"),0.);
            Condition* cd12 = new CompCondition(cd1,cd2,Relation::And);

            Constraint* c = new AC1(Var("dPair"),Relation::Greater,expr1);

            rule->addConditionDirectly(cd12);
            rule->addRuleDirectly(new RuleDynamic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }

        //if(!hasWindowLow && hasWindowHigh)
        {
            Condition* cd1 = new AtomCondition(Var("hasWindowLow"),0.);
            Condition* cd2 = new AtomCondition(Var("hasWindowHigh"),1.);
            Condition* cd12 = new CompCondition(cd1,cd2,Relation::And);

            Constraint* c = new AC2(Var("dPair"),Relation::Greater,expr2);

            rule->addConditionDirectly(cd12);
            rule->addRuleDirectly(new RuleDynamic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }
         //if(hasWindowLow)
        {
            Condition* cd = new AtomCondition(Var("hasWindowLow"),1.);

            Constraint* c = new AC2(Var("dPair"),Relation::Greater,expr3);

            rule->addConditionDirectly(cd);
            rule->addRuleDirectly(new RuleDynamic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf));
        }

        return rule;
    }



    case 3:
    {
        const char* strRule = "\n\
        if(hasWindowPair)  {dPair>=max(6,hHigh/2)}\n\
        if(!hasWindowPair) {dPair>=max(4,hHigh/2)}";

        typedef Expression<Constant> Literal;
        typedef Expression<Var> Variable;

        typedef BinaryExpression<Variable,Literal,Multiply> BE1;
        typedef BinaryExpression<Literal,BE1,Max> BE2;
        typedef Expression<BE2> Expr;
        typedef AtomConstraint<Expr> AC;

        Variable vH(Var("hHigh"));
        Literal l1(Constant(0.5));
        Literal l2(Constant(6.0));
        Literal l3(Constant(4.0));
        Expr expr1(BE2(l2,BE1(vH,l1)));
        Expr expr2(BE2(l3,BE1(vH,l1)));

        //if hasWindowPair
        Condition* condition1 = new AtomCondition(Var("hasWindowPair"),1.);
        Constraint* c1 = new AC(Var("dPair"),Relation::GreaterEqual,expr1);
        RuleEnergy* ruleEnergy1 = new RuleDynamic(c1,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);

        //if !hasWindowPair
        Condition* condition2 = new AtomCondition(Var("hasWindowPair"),0.);
        Constraint* c2 = new AC(Var("dPair"),Relation::GreaterEqual,expr2);
        RuleEnergy* ruleEnergy2 = new RuleDynamic(c2,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);

        Rule* rule = new Rule(RuleType::DistPair,strRule);
        rule->addConditionDirectly(condition1);
        rule->addConditionDirectly(condition2);
        rule->addRuleDirectly(ruleEnergy1);
        rule->addRuleDirectly(ruleEnergy2);
        return rule;

    }

    default:
        return 0;
    }

}


Rule* makeRule_hDiff(int iTest)
{
    const char* strRule = "hDiff<=6";

    typedef Expression<Constant> Literal;
    typedef AtomConstraint<Literal> AC;
    Constraint* c = new AC(Var("hDiff"),Relation::LessEqual,Literal(Constant(6.0)));

    RuleEnergy* ruleEnergy = new RuleStatic(c,1.0,EnergyFuncType::Zero,0.2,EnergyFuncType::Erf);

    Rule* rule = new Rule(RuleType::HeightDiff,strRule);
    rule->addRuleDirectly(ruleEnergy);
    return rule;
 }


Rule* makeRule_lcr(int iTest)
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
    case 3:
        strRule = "lcr<=0.8";
        c = new AC(Var("lcr"),Relation::LessEqual,Literal(Constant(0.8)));
        break;

    default:
        return 0;

    }

    RuleEnergy* ruleEnergy =  new RuleStatic(c,1.,EnergyFuncType::Zero,1,EnergyFuncType::Erf);

    Rule* rule = new Rule(RuleType::LCR,strRule);
    rule->addRuleDirectly(ruleEnergy);
    return rule;

 }


Rule* makeRule_far(int iTest)
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
    case 3:
        strRule = "far<=6";
        c = new AC(Var("far"),Relation::LessEqual,Literal(Constant(6.0)));
        break;
    default:
        strRule = 0;
        c = 0;
        return 0;
    }

    RuleEnergy* ruleEnergy =  new RuleStatic(c,0.1,EnergyFuncType::Square,1,EnergyFuncType::Erf);

    Rule* rule = new Rule(RuleType::FAR,strRule);
    rule->addRuleDirectly(ruleEnergy);
    return rule;
 }

RuleGeom* makeRuleGeom(int iTest)
{
    RuleGeom* rule = new RuleGeom();

    switch (iTest){
    case 1:
        rule->strH("[6,18]");
        rule->hMin(6.);
        rule->hMax(18.);
        rule->hFloor(3.);

        rule->strL("[20,30]");
        rule->lMin(20.);
        rule->lMax(30.);

        rule->strW("[10,16] peaks: 10,13,16");
        rule->wMin(10.);
        rule->wMax(16.);
        rule->add_widthPeak(10.);
        rule->add_widthPeak(13.);
        rule->add_widthPeak(16.);
        break;
    case 2:
        rule->strH("[3.,24]");
        rule->hMin(3.);
        rule->hMax(24.);
        rule->hFloor(3.);

        rule->strL("[20,50]");
        rule->lMin(20.);
        rule->lMax(50.);

        rule->strW("[5,10] peaks: 5,10");
        rule->wMin(5.);
        rule->wMax(10.);
        rule->add_widthPeak(5.);
        rule->add_widthPeak(10.);

        break;
    case 3:
        rule->strH("[3,16]");
        rule->hMin(3.);
        rule->hMax(16.);
        rule->hFloor(3.);

        rule->strL("[20,35]");
        rule->lMin(20.);
        rule->lMax(35.);

        rule->strW("[10,15]");
        rule->wMin(10.);
        rule->wMax(15.);
        break;

    default:
        break;
    }
    return rule;
}

int makeRule_nMax(int iTest)
{
    switch (iTest){
    case 1:
        return 4;
    case 2:
        return 6;
    case 3:
        return 8;
    default:
        return 0;
    }
}

double makeRule_lengthHasWindow(int iTest)
{
    switch (iTest){
    case 1:
    case 2:
    case 3:
        return 13.;
    default:
        return -1;
    }
}

bool isRectLike(int iTest)
{
    switch (iTest){
    case 1:
        return true;
    case 2:
        return false;
    case 3:
        return true;
    default:
        return false;
    }
}
