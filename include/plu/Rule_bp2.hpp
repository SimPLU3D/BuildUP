#ifndef RULE_HPP
#define RULE_HPP

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>


#define HMIN 3.2      //min height of each building (also min height of each floor)
#define HMAX 18 //3.2*6     //max height of each building
#define CESMAX 0.6  //coefficient d'emprise au sol: aire bâti/aire parcelle
#define COSMAX 3    //coefficient d'occupation des sols: aire plancher/aire parcelle
#define THETAMAX 3  //max angle (in degree) between building and lot normal
#define SIZEMIN 4  //min area of each building
#define RATIOMAX 5  //max ratio of length and width
#define DISTTOL_UR 2 //tolerent distance to the border of outer buildings
#define DISTMIN_UR 15 //min distance to the border of inner buildings
#define DISTMIN_BI 6 //min distance between buildings if there are windows
#define DISTMAX_BI 4 //max distance between buildings if no windows



    class RuleLot{
        public:
            RuleLot(): _hMin(HMIN), _hMax(HMAX), _cesMax(CESMAX), _cosMax{COSMAX}
            ,_thetaMax(THETAMAX),_sizeMin(SIZEMIN),_ratioMax(RATIOMAX)
            ,_distTolUr(DISTTOL_UR), _distMinUr(DISTMIN_UR), _distMinBi(DISTMIN_BI),_distMaxBi(DISTMAX_BI)
            {}

    //        RuleLot(double hMin, double hMax,double cesMax,double cosMax
    //        ,double thetaMax, double sizeMin, double ratioMax,double distMinUr, double distMinBi)
    //        : _hMin(hMin), _hMax(hMax), _cesMax(cesMax), _cosMax(cosMax)
    //        ,_thetaMax(thetaMax),_sizeMin(sizeMin),_ratioMax(ratioMax)
    //        ,_distMinUr(distMinUr),_distMinBi(distMinBi)
    //        {}
    //
    //        RuleLot(const RuleLot& o)
    //        {
    //            _hMin=o._hMin; _hMax = o._hMax; _cesMax = o._cesMax; _cosMax = o._cosMax;
    //            _thetaMax=o._thetaMax; _sizeMin=o._sizeMin;_ratioMax=o._ratioMax;
    //            _distMinUr=o._distMinUr; _distMinBi=o._distMinBi;
    //        }
    //
    //        RuleLot & operator = (const RuleLot & o)
    //        {
    //            _hMin=o._hMin; _hMax = o._hMax; _cesMax = o._cesMax; _cosMax = o._cosMax;
    //            _thetaMax=o._thetaMax; _sizeMin=o._sizeMin;_ratioMax=o._ratioMax;
    //            _distMinUr=o._distMinUr; _distMinBi=o._distMinBi;
    //            return *this;
    //        }

            ~RuleLot(){}

        public:
            double _hMin;
            double _hMax;
            double _cesMax;
            double _cosMax;
            double _thetaMax;
            double _sizeMin;
            double _ratioMax;
            double _distMinUr;
            double _distTolUr;
            double _distMinBi;
            double _distMaxBi;

    };

    class Envelope
    {
    public:
        Envelope(){}
        Envelope(double widthRd):_prospect(widthRd)
        {
            if(_prospect<8)
            {
                _h_main = _prospect+4;
                _slope1 = 1;
                _h_slope1 = 3;
                _slope2 = 0;
                _h_slope2 = 0;
                _radius = 0;
            }
            else if(_prospect>=8 && _prospect<12)
            {
                _h_main = _prospect+4;
                _slope1 = 2;
                _h_slope1 = 4.5;
                _slope2 = 0;
                _h_slope2 = 0;
                _radius = 0;
            }
            else if(_prospect>=12 && _prospect<20)
            {
                _h_main = _prospect+3;
                _slope1 = 2;
                _h_slope1 = 3;
                _slope2 = 1;
                _h_slope2 = 6;
                _radius = 0;
            }
            else if(_prospect>=20)
            {
                _h_main = (_prospect+3) > 25 ? 25:(_prospect+3);
                _slope1 = 0;
                _h_slope1 = 0;
                _slope2 = 0;
                _h_slope2 = 0;
                _radius = 6;
            }
            else
                std::cerr<<"no definition"<<std::endl;

        }
        ~Envelope(){}

        inline double getHMain(){return _h_main;}
        inline double getHTotal(){return _h_main+_h_slope1+_h_slope2;}

    public:
        double _prospect;
        double _h_main;
        double _slope1;
        double _h_slope1;
        double _slope2;
        double _h_slope2;
        double _radius;
    };

    class RuleRoad
    {
    public:
        RuleRoad(){}
        RuleRoad(double widthRd):_envelope(Envelope(widthRd)){}
        ~RuleRoad(){}

        inline double getHMain(){return _envelope.getHMain();}

    public:
        Envelope _envelope;
    };



namespace PLU{
    typedef enum {
        RT_DistanceFront    = 1<<0,
        RT_DistanceSide     = 1<<1,
        RT_DistanceBack     = 1<<2,
        RT_DistancePairwise = 1<<3,
        RT_CES              = 1<<4,
        RT_COS              = 1<<5,
        RT_Height           = 1<<6
    } RuleType;



    //////////////////////////////////////////////////////////////////

    typedef enum{
        EXT_Variable,
        EXT_Constant,
        EXT_Operation,
        EXT_Unknown
    } ExpressionType;

    struct Expression{
        virtual ~Expression(){}
        virtual double operator()(double){}
        virtual inline getExpressionType(void){return EXT_Unknown;}
    };

    struct Var: Expression
    {
        std::string _name;
        Var():_name("unknown var"){}
        Var(std::string name):_name(name){}
        Var(const char* name):_name(name){}
        double operator()(double v){return v;}
        inline getExpressionType(void){return EXT_Variable;}
    };

    struct Constant: Expression
    {
        double _c;
        Constant(double c):_c(c){}
        double operator()(double){return _c;}
        inline getExpressionType(void){return EXT_Constant;}
    };


    struct Operator{
        virtual inline double apply(double,double){}
    };

    struct Add:Operator{
        inline double apply(double v1,double v2){return v1+v2;}
    };

    struct Multiply:Operator {
        inline double apply(double v1,double v2){return v1*v2;}
    };

    struct Max:Operator{
        inline double apply(double v1,double v2){return std::max(v1,v2);}
    };

    struct Min:Operator{
        inline double apply(double v1,double v2){return std::min(v1,v2);}
    };

//support recursive binary operations with one variable
    struct Operation: Expression
    {
        Expression* _opd[2];
        Operator* _opt;

        Operation():_opd[0](NULL),_opd[1](NULL),_opt(NULL){}
        ~Operation(){
            if(_opd[0]!=NULL)
                delete _opd[0];
            if(_opd[1]!=NULL)
                delete _opd[1];
            if(_opt!=NULL)
                delete _opt;
        }

        double operator()(double x){return _opt->apply((*_opd[0])(x),(*_opd[1])(x));}
        inline getExpressionType(void){return EXT_Operation;}
    };

//    template<typename A,typename  B,typename OP>
//    struct BinaryExpression{
//        A _a;
//        B _b;
//        BinaryExpression(A a,B b):_a(a),_b(b){}
//        double operator()(double v){return OP::apply(_a(v),_b(v));}
//    };
//
//    template<typename E>
//    struct Expression{
//        E _expr;
//        Expression(E expr):_expr(expr){}
//        double operator()(double v){return _expr(v);}
//    };

    typedef enum{
        equal,
        greater,
        greater_equal,
        less,
        less_equal
    }Relation;

    class Constraint{
    public:
        Constraint(){}
        Constraint(Var& var,Relation r,Expression* pExpr):_var(var),_r(r),_pExpr(pExpr){}
        ~Constraint(){if (_pExpr!=NULL) delete _pExpr;}
        //return value of the expression
        double operator()(double x){return _pExpr(x);}
        Relation getRelation(void){return _r;}

    private:
        Var _var;
        Relation _r;
        Expression* _pExpr;
    };


    typedef boost::icl::continuous_interval<double> Interval;
    typedef std::numeric_limits<double> Limit;

    template<typename C>
    Interval constraintToInterval(C& constraint,double x)
    {
        double y = constraint(x);
        Relation r = constraint.getRelation();
        switch (r)
        {
            case equal:
                return Interval::closed(y,y);
            case greater:
                return Interval::open(y,Limit::infinity());
            case greater_equal:
                return Interval::right_open(y,Limit::infinity());
            case less: //plu constraints are >=0 implicitly
                return Interval::right_open(0,y);
            case less_equal:
                return Interval::closed(0,y);
        }
    }



    template<typename C1,typename C2>
    Interval constraintANDToInterval(C1& constraint1,double x1,C2& constraint2,double x2)
    {
        boost::icl::interval_map<double,int> intervals;
        intervals += std::make_pair(constraintToInterval(constraint1,x1),1);
        intervals += std::make_pair(constraintToInterval(constraint2,x2),1);

        if(intervals.iterative_size()==3)
            return (++intervals.begin())->first;
        std::cerr<<"error empty constraint AND";
    }


    class Rule{

        Constraint
    };
//    template<typename E> //E is an expression type
//    class Constraint{
//    public:
//        Constraint(Relation r,E expr):_r(r),_expr(expr){}
//
//        //get value of the expression; now only support expression with one variable
//        double operator()(double x){return _expr(x);}
//
//        Relation getRelation(void){return _r;}
//
//    private:
//
//        Relation _r;
//        E _expr;
//    };
//
//
//    typedef boost::icl::continuous_interval<double> Interval;
//    typedef std::numeric_limits<double> Limit;
//
//    template<typename C>
//    Interval constraintToInterval(C& constraint,double x)
//    {
//        double y = constraint(x);
//        Relation r = constraint.getRelation();
//        switch (r)
//        {
//            case equal:
//                return Interval::closed(y,y);
//            case greater:
//                return Interval::open(y,Limit::infinity());
//            case greater_equal:
//                return Interval::right_open(y,Limit::infinity());
//            case less: //plu constraints are >=0 implicitly
//                return Interval::right_open(0,y);
//            case less_equal:
//                return Interval::closed(0,y);
//        }
//    }
//
//
//
//    template<typename C1,typename C2>
//    Interval constraintANDToInterval(C1& constraint1,double x1,C2& constraint2,double x2)
//    {
//        boost::icl::interval_map<double,int> intervals;
//        intervals += std::make_pair(constraintToInterval(constraint1,x1),1);
//        intervals += std::make_pair(constraintToInterval(constraint2,x2),1);
//
//        if(intervals.iterative_size()==3)
//            return (++intervals.begin())->first;
//        std::cerr<<"error empty constraint AND";
//    }

}//namespace PLU

#endif // RULE_HPP
