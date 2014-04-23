#ifndef PLU_GLOBAL_ENERGY_HPP_INCLUDED
#define PLU_GLOBAL_ENERGY_HPP_INCLUDED

#include "plu/Lot.hpp"
#include <gdal/ogrsf_frmts.h>
#include <CGAL/number_utils_classes.h>
#include <CGAL/Simple_cartesian.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_set_2.h>
//#include <CGAL/Boolean_set_operations_2.h>

#include <vector>
#include <list>

#include "io/osg.hpp"


template<typename Value = double>
class plu_global_ces_simple : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Configuration, typename Modification>
    result_type operator()(const Configuration &c, const Modification &modif) const
    {
        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
        bci bbeg = modif.birth_begin();
        bci bend = modif.birth_end();
        dci dbeg = modif.death_begin();
        dci dend = modif.death_end();

        double area=0;
        typename Configuration::const_iterator it;
        for(it=c.begin();it!=c.end();++it)
        {
            if (std::find(dbeg,dend,it)!=dend) //dead
                continue;
            area+=c[it].area();
        }

        for(bci it2=bbeg;it2!=bend;++it2)
        {
            //if((*it2).hasBadEdge(2)==true)
               // continue;
            area+=(*it2).area();
        }

//        if(area<=0 || isinf(area))
//            return _eRej;

        double ces = area/_lot->_area;

        if(ces!=ces || std::isinf(ces) || ces<0)
            return _eRej;

        double diff = _lot->_rule._cesMax - ces;

        if( diff >= 0)
            return 0;
            //return _eRej*boost::math::erf(diff);
//            return _eRej*diff*diff;

        return _eRej*std::abs(boost::math::erf(2*diff));



//        std::map<Var,double> varValue;
//        varValue.insert(std::make_pair(Var("ces"),ces));
//        double e = _eRej*(_lot->_rules[RT_CES])->energy(varValue);
//
////        std::cout<<"ces "<<ces<<"\n";
////        std::cout<<"energy 2:"<<e<<"\n";
////        std::cout<<"erf:"<<_eRej*std::abs(boost::math::erf(ces-_lot->_rule._cesMax))<<"\n";
//
//
//        return e;

    }
    plu_global_ces_simple(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej){}

private:
    Lot* _lot;
    Value _eRej;
};


template<typename Value = double>
class plu_global_cos_simple : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Configuration, typename Modification>
    result_type operator()(const Configuration &c, const Modification &modif) const
    {
        //return 0;
        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
        bci bbeg = modif.birth_begin();
        bci bend = modif.birth_end();
        dci dbeg = modif.death_begin();
        dci dend = modif.death_end();

        double areaF=0,hF = _lot->_rule._hFloor;
        typename Configuration::const_iterator it;
        for(it=c.begin();it!=c.end();++it)
        {
            if (std::find(dbeg,dend,it)!=dend) //dead
                continue;
            areaF+=c[it].area()*(int)(c[it].h/hF);
        }

        for(bci it2=bbeg;it2!=bend;++it2)
            areaF+=(*it2).area()*(int)((*it2).h/hF);

        double cos = areaF/_lot->_area;

        if(cos!=cos || std::isinf(cos) || cos<0)
            return _eRej;

        double diff = _lot->_rule._cosMax - cos;
        if( diff >= 0)
            return _eRej*boost::math::erf(0.2*diff);

        return _eRej*std::abs(boost::math::erf(diff));

    }
    plu_global_cos_simple(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej){}

private:
    Lot* _lot;
    Value _eRej;

};



template<typename Value = double>
class plu_global_ces : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename Configuration, typename Modification>
    result_type operator()(const Configuration &c, const Modification &modif) const
    {
        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
        bci bbeg = modif.birth_begin();
        bci bend = modif.birth_end();
        dci dbeg = modif.death_begin();
        dci dend = modif.death_end();

        OGRMultiPolygon* plys = new OGRMultiPolygon;

        typename Configuration::const_iterator   it;
        for(it=c.begin();it!=c.end();++it)
        {
            if(c[it].hasBadVertex()==true || c[it].hasBadEdge()==true )
                return _eRej;
            if (std::find(dbeg,dend,it)!=dend) //dead
                continue;

            OGRLinearRing ring;
            for(int i=0;i<4;++i)
            {
                double x = CGAL::to_double(c[it].rect_2.point(i).x());
                double y = CGAL::to_double(c[it].rect_2.point(i).y());
                ring.addPoint(x,y);
            }

            OGRPolygon* ply = new OGRPolygon;
            ply->addRing(&ring);
            ply->closeRings();
            plys->addGeometryDirectly(ply);
        }

        for(bci it2=bbeg;it2!=bend;++it2)
        {
            if((*it2).hasBadVertex()==true|| (*it2).hasBadEdge()==true )
                return _eRej;

            OGRLinearRing ring;
            for(int i=0;i<4;++i)
            {
                double x = CGAL::to_double((*it2).rect_2.point(i).x());
                double y = CGAL::to_double((*it2).rect_2.point(i).y());
                ring.addPoint(x,y);
            }

            OGRPolygon* ply = new OGRPolygon;
            ply->addRing(&ring);
            ply->closeRings();
            plys->addGeometryDirectly(ply);
        }

        if(!plys->getNumGeometries())
        {
            plys->empty();
           // std::cout<<"empty config"<<std::endl;
            return _eRej;
        }

       // io::display(plys);
        OGRGeometry* fp_union = plys->UnionCascaded();
        double area = 0;
        if(fp_union->getGeometryType() == wkbPolygon)
            area=((OGRPolygon*)fp_union)->get_Area();
        else if(fp_union->getGeometryType() == wkbMultiPolygon)
            area=((OGRMultiPolygon*)fp_union)->get_Area();
        else
            std::cout<<"error: union result problem"<<std::endl;

        plys->empty();
        fp_union->empty();

        double ces = area/_lot->_area;

        if(ces!=ces || std::isinf(ces) || ces<0)
            return _eRej;


        double diff = _lot->_rule._cesMax - ces;

        //if( diff >= 0)
            //return _eRej*boost::math::erf(diff);
        //return _eRej*diff*diff;

        return _eRej*std::abs(boost::math::erf(diff));


        std::map<Var,double> varValue;
        varValue.insert(std::make_pair(Var("ces"),ces));
        double e = _eRej*(_lot->_rules[RT_CES])->energy(varValue);
        std::cout<<"ces "<<ces<<" "<<"energy "<<e<<"\n";
        return e;


    }



    plu_global_ces(Lot* lot,Value eRej) : _lot(lot),_eRej(eRej){}

private:
    Lot* _lot;
    Value _eRej;
};

template<typename Value = double>
class plu_global_cos : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    bool check_polygon_validity(const OGRGeometry* p) const
    {
        if(p->getGeometryType()!= wkbPolygon)
            return false;
        OGRPolygon* ply = (OGRPolygon*) p;

        if(!ply->IsValid())
            return false;
        if(!ply->IsSimple())
            return false;
        if(ply->getExteriorRing()->isClockwise())
        {
//            for(int i=0;i<ply->getExteriorRing()->getNumPoints();++i)
//                std::cout<<ply->getExteriorRing()->getX(i)<<","<<ply->getExteriorRing()->getY(i)<<std::endl;
            return false;
        }
        for(int i=0;i<ply->getNumInteriorRings();++i)
        {
            if(!ply->getInteriorRing(i)->isClockwise())
                return false;
                //std::cout<<"wrong winding: interior ring"<<std::endl;
        }

//        if(ply->get_Area()<2 || ply->get_Area()>_lot->_area)
//            return false;

        return true;
    }

    template<typename Configuration, typename Modification>
    result_type operator()(const Configuration &c, const Modification &modif) const
    {

        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
        bci bbeg = modif.birth_begin();
        bci bend = modif.birth_end();
        dci dbeg = modif.death_begin();
        dci dend = modif.death_end();

        std::vector<OGRGeometry*> plys;
        std::vector<double> heights;

        typename Configuration::const_iterator it;
        for(it=c.begin();it!=c.end();++it)
        {

            if (std::find(dbeg,dend,it)!=dend) //dead
                continue;

//            if(c[it].hasBadVertex()==true || c[it].hasBadEdge()==true )
//                return _eRej;

            OGRLinearRing ring;
            for(int i=0;i<4;++i)
            {
                double x = CGAL::to_double(c[it].rect_2.point(i).x());
                double y = CGAL::to_double(c[it].rect_2.point(i).y());
                ring.addPoint(x,y);
            }

            OGRPolygon* ply = new OGRPolygon;
            ply->addRing(&ring);
            ply->closeRings();
//            if(check_polygon_validity(ply)==false)
//            {
//                ply->empty();
//                return _eRej;
//            }
            plys.push_back(ply);
            heights.push_back(c[it].h);
        }


        for(bci it2=bbeg;it2!=bend;++it2)
        {
            if((*it2).hasBadVertex()==true|| (*it2).hasBadEdge()==true )
                return _eRej;

            OGRLinearRing ring;
            for(int i=0;i<4;++i)
            {
                double x = CGAL::to_double((*it2).rect_2.point(i).x());
                double y = CGAL::to_double((*it2).rect_2.point(i).y());
                ring.addPoint(x,y);
            }

            OGRPolygon* ply = new OGRPolygon;
            ply->addRing(&ring);
            ply->closeRings();
            if(check_polygon_validity(ply)==false)
            {
                ply->empty();
                return _eRej;
            }
            plys.push_back(ply);
            heights.push_back((*it2).h);
        }


        if(plys.size()==0)
            return _eRej;


        std::vector<int> drops;
        for(int i=1;i<plys.size();++i)
        {
            for(int j=0;j<i;++j)
            {
                if(std::find(drops.begin(),drops.end(),j)!=drops.end())
                    continue;

//                if(plys.at(i)->Contains(plys.at(j))|| plys.at(i)->Within(plys.at(j)))
//                    return _eRej;

                if(plys.at(i)->Disjoint(plys.at(j)) || plys.at(i)->Touches(plys.at(j)))
                    continue;

                if(plys.at(i)->Within(plys.at(j))&& heights[i]<heights[j])
                {
                    //std::cout<<"within"<<std::endl;
                    drops.push_back(i);
                    continue;
                }
                if(plys.at(i)->Contains(plys.at(j))&& heights[i]>=heights[j])
                {
                    //std::cout<<"contains"<<std::endl;
                    drops.push_back(j);
                    continue;
                }

                int low = heights[i]<heights[j]? i:j;
                int high = heights[i]>=heights[j]? i:j;

                OGRGeometry * remain;
                remain = plys.at(low)->Difference(plys.at(high));

                if(remain->getGeometryType()==wkbGeometryCollection || remain->getGeometryType()==wkbMultiPolygon)
                {
                    OGRGeometryCollection* g = (OGRGeometryCollection*)remain;
                    for(int k=0;k<g->getNumGeometries();++k)
                    {
                        if(check_polygon_validity(g->getGeometryRef(k))==false)
                        {
                            g->removeGeometry(k);
                            k--;
                        }
                    }
                    if(g->getNumGeometries()==0)
                    {
                        drops.push_back(low);
                        remain->empty();
                        continue;
                    }

                    plys[low] = g->getGeometryRef(0)->clone();

                    if(g->getNumGeometries()>1)
                        for(int k=1;k<g->getNumGeometries();++k)
                        {
                            plys.push_back(g->getGeometryRef(k)->clone());
                            heights.push_back(heights[low]);
                        }

                    remain->empty();
                    continue;
                }

                if(remain->getGeometryType()==wkbPolygon)
                {
                    if(check_polygon_validity(remain)==false)
                    {
                        drops.push_back(low);
                        remain->empty();
                        continue;
                    }
                    plys[low]->empty();
                    plys[low]=remain;
                    continue;
                }

                drops.push_back(low);
                remain->empty();
            }
        }

        double areaF=0., hF = _lot->_rule._hFloor;
        for(int i=0;i<plys.size();++i)
        {
            if(std::find(drops.begin(),drops.end(),i)!=drops.end())
                continue;

            double a = ((OGRPolygon*)plys[i])->get_Area();

            areaF += a*((int)(heights.at(i)/hF));
            plys.at(i)->empty();

        }

        if(areaF==0)
            return _eRej;
        double cos = areaF/_lot->_area;
        double MaxCOS = _lot->_rule._cosMax;
        return MaxCOS >= cos? (MaxCOS-cos)*_coef:_eRej;

    }

//    result_type operator()(const Configuration &c, const Modification &modif) const
//    {
//
//        typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
//        typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
//        bci bbeg = modif.birth_begin();
//        bci bend = modif.birth_end();
//        dci dbeg = modif.death_begin();
//        dci dend = modif.death_end();
//
//        typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
//        //typedef CGAL::Simple_cartesian<double> Kernel;
//        typedef Kernel::Point_2 Point;
//        typedef CGAL::Polygon_2<Kernel> Polygon_2;
//        typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
//        typedef CGAL::Polygon_set_2<Kernel> Polygon_set_2;
//
//        std::vector<Polygon_set_2> plySets;
//        std::vector<double> heights;
//
//        typename Configuration::const_iterator it;
//        for(it=c.begin();it!=c.end();++it)
//        {
//            if(c[it].hasBadVertex()==true || c[it].hasBadEdge()==true )
//                return _eRej;
//            if (std::find(dbeg,dend,it)!=dend) //dead
//                continue;
//
//            Polygon_2 ply;
//            for(int i=0;i<4;++i)
//                ply.push_back(Point(c[it].rect_2.point(i).x(),c[it].rect_2.point(i).y()));
//
//            Polygon_set_2 s;
//            s.insert(ply);
//            plySets.push_back(s);
//            heights.push_back(c[it].h);
//        }
//
//        for(bci it2=bbeg;it2!=bend;++it2)
//        {
//            if((*it2).hasBadVertex()==true|| (*it2).hasBadEdge()==true )
//                return _eRej;
//
//            Polygon_2 ply;
//            for(int i=0;i<4;++i)
//                ply.push_back(Point((*it2).rect_2.point(i).x(),(*it2).rect_2.point(i).y()));
//
//            Polygon_set_2 s;
//            try{
//                s.insert(ply);
//            }
//            catch(std::exception& ex){
//                std::cout<<"not simple"<<std::endl;
//                typename Polygon_2::Vertex_const_iterator vit;
//                std::cout << "[ " << ply.size() << " vertices:";
//                for (vit = ply.vertices_begin(); vit != ply.vertices_end(); ++vit)
//                std::cout << " (" << *vit << ')';
//                std::cout << " ]" << std::endl;
//                return _eRej;
//            }
//            plySets.push_back(s);
//            heights.push_back((*it2).h);
//        }
//
//
//        if(plySets.size()==0)
//            return _eRej;
//
//        for(int i=1;i<plySets.size();++i)
//        {
//            for(int j=0;j<i;++j)
//            {
//                if(plySets[i].do_intersect(plySets[j]))
//                {
//                    int low = heights[i]<heights[j]? i:j;
//                    int high = heights[i]>=heights[j]? i:j;
//                    plySets[low].difference(plySets[high]);
//                }
//            }
//
//        }
//
//        double areaF=0., hF = _lot->_rule._hMin;
//        for(int i=0;i<plySets.size();++i)
//        {
//            std::list<Polygon_with_holes_2> res;
//            std::list<Polygon_with_holes_2>::const_iterator it;
//            plySets[i].polygons_with_holes(std::back_inserter(res));
//            double area = 0;
//            for (it = res.begin(); it != res.end(); ++it)
//            {
//                if(it->is_plane())
//                    continue;
//                area += CGAL::to_double(it->outer_boundary().area());
//                if(it->has_holes())
//                    std::cout<<"has hole"<<std::endl;
////                {
////                    typename Polygon_with_holes_2::Hole_iterator hit;
////                    for(hit=it->holes_begin();hit!=it->holes_end();++hit)
////                    {
////                        double areaHole = CGAL::to_double(hit->area());
////                        if(areaHole>=0)
////                            area-=areaHole;
////                        else
////                            area+=areaHole;
////                    }
////                }
//            }
//
//            areaF += area*((int)(heights.at(i)/hF));
//        }
//        if(areaF==0)
//            return _eRej;
//
//        double cos = areaF/_lot->_area;
//        double MaxCOS = _lot->_rule._cosMax;
//        return MaxCOS >= cos? (MaxCOS-cos)*_coef:_eRej;
//
//    }

    plu_global_cos(Lot* lot,Value eRej, Value coef) : _lot(lot),_eRej(eRej),_coef(coef){}

private:
    Lot* _lot;
    Value _eRej;
    Value _coef;
};

#endif // PLU_GLOBAL_ENERGY_HPP_INCLUDED
