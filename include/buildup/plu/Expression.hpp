#ifndef EXPRESSION_HPP_INCLUDED
#define EXPRESSION_HPP_INCLUDED
#include <algorithm>
#include <map>

struct Var
{
    std::string _name;
    Var():_name("unknown name") {}
    Var(std::string name):_name(name) {}
    Var(const char* name):_name(name) {}

    inline bool operator == (const Var& o)const{return !(_name.compare(o._name));}
    inline bool operator < (const Var& o) const{return _name.compare(o._name)<0;}
    inline bool operator > (const Var& o) const{return _name.compare(o._name)>0;}

    inline double operator()(std::map<Var,double>& v)
    {
        std::map<Var,double>::iterator it = v.find(*this);
        if(it == v.end()){
            std::cerr<<"error no matched variable for rvalue"<<_name<<"\n";
            exit(2);
        }
        return it->second;
    }
};

typedef std::map<Var,double> VarValue;

struct Constant
{
    double _c;
    Constant(double c):_c(c) {}
    inline double operator()(VarValue&) const {return _c;}
//    inline double operator()(double) const {return _c;}
//    inline double operator()() const{return _c;}
};

struct Add{static double apply(double v1,double v2){return v1+v2;}};
struct Multiply{static double apply(double v1,double v2){return v1*v2;}};
struct Max{static double apply(double v1,double v2){return std::max(v1,v2);}};
struct Min{static double apply(double v1,double v2){return std::min(v1,v2);}};

template<typename A,typename  B,typename OP>
struct BinaryExpression
{
    A _a;
    B _b;
    BinaryExpression(A a,B b):_a(a),_b(b) {}
    inline double operator()(VarValue& v){return OP::apply(_a(v),_b(v));}
};

template<typename E>
struct Expression
{
    E _expr;
    Expression(E expr):_expr(expr) {}
    inline double operator()(VarValue& v){return _expr(v);}
};


enum class Relation
{
    Equal,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    And,
    Or,
    Unknown
};

#endif // EXPRESSION_HPP_INCLUDED
