#ifndef EXPRESSION_HPP_INCLUDED
#define EXPRESSION_HPP_INCLUDED
#include <algorithm>

struct Var
{
    std::string _name;
    Var():_name("unknown name") {}
    Var(std::string name):_name(name) {}
    Var(const char* name):_name(name) {}
    inline double operator()(double v)
    {
        return v;
    }
    inline bool operator == (const Var& o)const
    {
        return !(_name.compare(o._name));
    }
    inline bool operator < (const Var& o) const
    {
        return _name.compare(o._name)<0;
    }
    inline bool operator > (const Var& o) const
    {
        return _name.compare(o._name)>0;
    }
};

struct Constant
{
    double _c;
    Constant(double c):_c(c) {}
    double operator()(double)
    {
        return _c;
    }
};


struct Add
{
    static double apply(double v1,double v2)
    {
        return v1+v2;
    }
};

struct Multiply
{
    static double apply(double v1,double v2)
    {
        return v1*v2;
    }
};

struct Max
{
    static double apply(double v1,double v2)
    {
        return std::max(v1,v2);
    }
};

struct Min
{
    static double apply(double v1,double v2)
    {
        return std::min(v1,v2);
    }
};

template<typename A,typename  B,typename OP> //support only one variable
struct BinaryExpression
{
    A _a;
    B _b;
    BinaryExpression(A a,B b):_a(a),_b(b) {}
    double operator()(double v)
    {
        return OP::apply(_a(v),_b(v));
    }
};

template<typename E>
struct Expression
{
    E _expr;
    Expression(E expr):_expr(expr) {}
    double operator()(double v)
    {
        return _expr(v);
    }
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
