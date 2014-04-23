#ifndef CONSTRAINT_HPP_INCLUDED
#define CONSTRAINT_HPP_INCLUDED

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_map.hpp>
#include "Expression.hpp"
#include "Energy.hpp"


    typedef enum{
        CT_Atomic,
        CT_Composite,
        CT_Unknown
    }ConstraintType;


    class Constraint{
    public:
        //Constraint():_leftChild(NULL),_rightChild(NULL),_parent(NULL){}
        Constraint(Constraint* lc,Constraint* rc, Relation r):_leftChild(lc),_rightChild(rc),_r(r){}
        virtual ~Constraint(){if (_leftChild!=NULL) delete _leftChild; if(_rightChild!=NULL) delete _rightChild;}

        typedef boost::icl::continuous_interval<double> Interval;
        typedef std::vector<Interval> Intervals;
        typedef boost::icl::interval_map<double,int> IntervalMap;
        typedef std::numeric_limits<double> Limit;


        virtual inline Var getVar(void){}
        virtual inline bool isForVar(Var){}
        virtual inline bool isForSameVar(void){}
        virtual Intervals toInterval(double){}
        virtual EnergyPLU* toEnergy(double){}


        inline Constraint* getLeftChild(void){return _leftChild;}
        inline Constraint* getRightChild(void){return _rightChild;}
        inline Relation getRelation(void){return _r;}


    protected:
        Constraint* _leftChild;
        Constraint* _rightChild;
        Relation _r;
    };

    template<typename E> //E is an expression type
    class AtomConstraint:public Constraint{
    public:
        //AtomConstraint(){}
        AtomConstraint(Var v,Relation r,E expr):Constraint(NULL,NULL,r),_var(v),_expr(expr){}

        inline Var getVar(void){return _var;}
        inline bool isForVar(Var v){return _var==v;}
        inline bool isForSameVar(void){return true;}

        Intervals toInterval(double x)
        {
            Intervals intervals;
            double y = _expr(x);
            switch (_r)
            {
                case RL_Equal:
                    intervals.push_back( Interval::closed(y,y));
                    break;
                case RL_Greater:
                    intervals.push_back( Interval::open(y,Limit::infinity()));
                    break;
                case RL_GreaterEqual:
                    intervals.push_back( Interval::right_open(y,Limit::infinity()));
                    break;
                case RL_Less: //plu constraints are >=0 implicitly
                    intervals.push_back( Interval::right_open(0,y));
                    break;
                case RL_LessEqual:
                    intervals.push_back( Interval::closed(0,y));
                    break;
                default:
                    std::cerr<<"error: wrong relation type";
            }
            return intervals;
        }

        EnergyPLU* toEnergy(double x)
        {
            EnergyPLU*e = new EnergyPiecewise(_var,toInterval(x));
            return e;
        }

    private:
        Var _var;
        E _expr;
    };




    class CompConstraint: public Constraint{
    public:
        //CompConstraint():_r(RL_Unknown){_c[0]=NULL;_c[c1]=NULL;}
        CompConstraint(Constraint* lc, Constraint* rc,Relation r):Constraint(lc,rc,r){}

        inline Var getVar(void){return _leftChild->getVar();}//leftest var of its children
        inline bool isForVar(Var v){return _rightChild->isForVar(v) && _leftChild->isForVar(v);}
        inline bool isForSameVar(void){return _leftChild->isForVar(_leftChild->getVar()) && _rightChild->isForVar(_leftChild->getVar());}

        Intervals toInterval(double x)
        {
            if(!isForSameVar())
                std::cerr<<"error: can not convert to intervals for different variables";

            switch(_r)
            {
            case RL_And:
                return intervalAND(_leftChild->toInterval(x),_rightChild->toInterval(x));
            case RL_Or:
                return intervalOR(_leftChild->toInterval(x),_rightChild->toInterval(x));
            default:
                std::cerr<<"error: wrong logic";
            }
        }

        EnergyPLU* toEnergy(double x)
        {
            EnergyPLU* e;
            if(isForSameVar())
                e = new EnergyPiecewise(getVar(),toInterval(x));
            else
                e = new EnergyComposite(_leftChild->toEnergy(x),_rightChild->toEnergy(x),_r);

            return e;
        }

    private:

        Intervals intervalOR(Intervals is1,Intervals is2)
        {
            Intervals out;
            IntervalMap iMap;
            Intervals::iterator itV;
            for(itV=is1.begin();itV!=is1.end();++itV)
                iMap += std::make_pair(*itV,1);
            for(itV=is2.begin();itV!=is2.end();++itV)
                iMap += std::make_pair(*itV,1);

            Intervals overlaps;
            IntervalMap::iterator itMap;
            for(itMap=iMap.begin();itMap!=iMap.end();++itMap)
                if(itMap->second == 2)
                    overlaps.push_back(itMap->first);
            for(itV=overlaps.begin();itV!=overlaps.end();++itV)
                iMap += std::make_pair(*itV,-1);

            for(itMap=iMap.begin();itMap!=iMap.end();++itMap)
                out.push_back(itMap->first);

            return out;

        }
        Intervals intervalAND(Intervals is1,Intervals is2)
        {
            Intervals out;
            IntervalMap iMap;
            Intervals::iterator itV;
            for(itV=is1.begin();itV!=is1.end();++itV)
                iMap += std::make_pair(*itV,1);
            for(itV=is2.begin();itV!=is2.end();++itV)
                iMap += std::make_pair(*itV,1);

            IntervalMap::iterator itMap;
            for(itMap = iMap.begin();itMap!=iMap.end();++itMap)
                if(itMap->second == 2)
                    out.push_back(itMap->first);
            return out;
        }
    };



#endif // CONSTRAINT_HPP_INCLUDED
