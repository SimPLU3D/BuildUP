#ifndef LOT_HPP
#define LOT_HPP

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "Rule.hpp"
//#include <SFCGAL/all.h>
#include <gdal/ogrsf_frmts.h>
#include <map>



enum BorderType{
    FrontBorder,
    SideBorder,
    BackBorder,
    Unknown
};

class BorderSeg{
    public:
        typedef geometry::Simple_cartesian<double> K;
        typedef typename K::Point_2 Point_2;
        typedef typename K::Segment_2 Segment_2;


        BorderSeg(){}
        BorderSeg(int id_lot,int id,Segment_2& seg): _id_lot(id_lot),_id(id),_seg(seg),_type(Unknown),_id_border(-1){}

        inline int getID(){return _id;}
        inline Segment_2& getGeom(){return _seg;}
        inline BorderType getType(){return _type;}
        inline int getBorderID(){return _id_border;}

        inline void setGeom(Segment_2& seg){_seg=seg;}
        inline void setType(BorderType t){_type=t;}
        inline void setBorderID(int i){_id_border=i;}

        inline double squared_dist(const Point_2& p){return _seg.squared_distance2point(p);}

    private:
        int _id_lot;
        int _id; //unique id in each lot
        Segment_2 _seg;
        //values to be assigned manually:
        BorderType _type;
        int _id_border; //belongs to which border
};


class Border{
public:
    typedef geometry::Simple_cartesian<double> K;
    typedef typename K::Segment_2 Segment_2;

    Border(){}
    Border(int id_lot,int id,BorderType type):_id_lot(id_lot),_id(id),_type(type){}

    inline void addSeg(BorderSeg* pSeg){_pSegs.push_back(pSeg);}

    inline std::vector<BorderSeg*>& getSegs(){return _pSegs;}
    inline Segment_2& getSeg(int i){return _pSegs.at(i)->getGeom();}
    inline int getID(){return _id;}
    inline BorderType getType(){return _type;}

private:
    int _id_lot;
    int _id; //unique id in each lot
    BorderType _type;
    std::vector<BorderSeg*> _pSegs;

};



class Lot{
    public:
        typedef geometry::Simple_cartesian<double> K;
        typedef typename geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
        typedef typename K::Point_2 Point_2;
        typedef typename K::Vector_2 Vector_2;
        typedef typename K::Segment_2 Segment_2;


        Lot(){_polygon = new OGRPolygon;}

        Lot(int id, OGRPolygon* ply,RuleLot ruleLot = RuleLot(), RuleRoad ruleRd = RuleRoad())
        : _id(id),_polygon((OGRPolygon*)(ply->clone())),_area(ply->get_Area())
        ,_rule(ruleLot),_ruleRd(ruleRd)
        {
            _polygon->getEnvelope(&_box2d);
            extractBorderSegs();
        }

        Lot(const Lot& o)
        {
            _id = o._id;
            _polygon = (OGRPolygon*)(o._polygon->clone());
            _area = o._area;
            _box2d = o._box2d;
            _borderSegs = o._borderSegs;
        }
//        Lot(OGRPolygon* ply, RuleLot& rule, RuleRoad& ruleRd,Iso_rectangle_2& box)
//        : _polygon((OGRPolygon*)(ply->clone())),_area(ply->get_Area())
//        , _rule(rule),_ruleRd(ruleRd), _box2d(box)
//        //find the longest edge as the main edge
//        {set_mainEdge_longest();}

//         Lot(OGRPolygon* ply, RuleLot& rule, Iso_rectangle_2& box,Segment_2& mainEdge)
//        : _polygon((OGRPolygon*)(ply->clone())), _rule(rule),_box2d(box),_area(ply->get_Area())
//        ,_mainEdge(mainEdge),_normal(_mainEdge.target()-_mainEdge.source()){}

        ~Lot(){_polygon->empty();

            std::map< RuleType, Rule*>::iterator it;
            for(it=_rules.begin();it!=_rules.end();++it)
                if( it->second!= NULL) delete it->second;

        }

//        Vector_2 getBoxNormal()
//        {
//            double dx=_box2d.MaxX-_box2d.MinX;
//            double dy=_box2d.MaxY-_box2d.MinY;
//            return dx>dy? Vector_2(dx/2,0):Vector_2(0,dy/2);
//        }


        void translate(double x0,double y0)
        {
            //1) lot polygon
            //2) lot envelope
            //3) border segments
            int n=_polygon->getExteriorRing()->getNumPoints();
            for(int i=0;i<n;++i)
            {
                double x = _polygon->getExteriorRing()->getX(i)-x0;
                double y = _polygon->getExteriorRing()->getY(i)-y0;
                _polygon->getExteriorRing()->setPoint(i,x,y);
            }

            _polygon->getEnvelope(&_box2d);

            Vector_2 pt0(x0,y0);
            std::map< int,BorderSeg>::iterator it;
            for(it=_borderSegs.begin();it!=_borderSegs.end();++it)
            {
                Segment_2 seg(it->second.getGeom().source()-pt0,it->second.getGeom().target()-pt0);
                it->second.setGeom(seg);
            }
        }
//
//        void setMainEdgeNormal()
//        {
//            if(_frontBorders.empty())
//                set_mainEdge_longest();
//            else
//            {
//                _mainEdge = _borderSegs[_frontBorders[0]]._geom;
//                _normal = _mainEdge.target()-_mainEdge.source();
//            }
//        }

        void setRefSeg(bool isRectLike)
        {
            _isRectLike = isRectLike;
            if(_isRectLike==true)
            {
                std::map<int,BorderSeg>::iterator it;
                for(it=_borderSegs.begin();it!=_borderSegs.end();++it)
                    if(it->second.getType()==FrontBorder)
                    {
                        _idRefSeg = it->second.getID();
                        _normal = getRefSeg().target()-getRefSeg().source();
                        return;
                    }
            }
            else
                _idRefSeg = -1;
        }

        inline Segment_2& getRefSeg(){return _borderSegs.find(_idRefSeg)->second.getGeom();}
        inline double getRefTheta(){return std::atan2(_normal.y(),_normal.x());}


        template<typename OBJ>
        void dist2borders(OBJ& obj,std::map< BorderType,std::map<int,double> >& result)
        {
            //<BorderType, unique id per type, pt_Border>
            std::map< BorderType,std::map<int,Border*> > type_borders;

            //form type_borders
            std::map< int,Border>::iterator it;
            int id_per_type = 0;
            for(it=_borders.begin();it!=_borders.end();++it)
            {
                if(type_borders.find(it->second.getType())==type_borders.end())
                    id_per_type = 0;
                type_borders[it->second.getType()][id_per_type] = &(it->second);
                id_per_type++;
            }


            std::map<BorderType,std::map<int,Border*> >::iterator it1;
            std::map<int,Border*>::iterator it2;

            for(it1=type_borders.begin();it1!=type_borders.end();++it1)
            {
                std::map<int,Border*>& borders = it1->second;
                for(it2=borders.begin();it2!=borders.end();++it2)
                {
                    std::vector<BorderSeg*>& segs = it2->second->getSegs();
                    int n = segs.size();
                    if(n<1)
                       std::cerr<<"empty border ";

                    double d2min = segs.at(0)->squared_dist(obj.center());
                    int idNearest = segs.at(0)->getID();
                    for(int i=1;i<n;++i)
                    {
                        double d2 = segs.at(i)->squared_dist(obj.center());
                        if(d2<d2min)
                        {
                            d2min=d2;
                            idNearest=segs.at(i)->getID();
                        }
                    }

                    double dist = obj.distance2line(_borderSegs[idNearest].getGeom());
                    result[it1->first][it2->first] = dist;
                }

            }
        }



    private:
        void extractBorderSegs()//called by constructor
        {
            int idSeg = -1;
            OGRLinearRing* ring = _polygon->getExteriorRing();
            for(int i=0;i<ring->getNumPoints()-1;++i)
            {
                Point_2 pt1(ring->getX(i),ring->getY(i));
                Point_2 pt2(ring->getX(i+1),ring->getY(i+1));
                ++idSeg;
                Segment_2 s(pt1,pt2);
                _borderSegs[idSeg]=BorderSeg(_id,idSeg,s);
            }

            for(int j=0;j<_polygon->getNumInteriorRings();++j)
            {
                ring = _polygon->getInteriorRing(j);
                for(int i=0;i<ring->getNumPoints()-1;++i)
                {
                    Point_2 pt1(ring->getX(i),ring->getY(i));
                    Point_2 pt2(ring->getX(i+1),ring->getY(i+1));
                    ++idSeg;
                    Segment_2 s(pt1,pt2);
                    _borderSegs[idSeg]=BorderSeg(_id,idSeg,s);

                }
            }
        }

    public:
        int _id;
        OGRPolygon* _polygon;
        double _area;
        OGREnvelope _box2d;

        RuleLot _rule;
        RuleRoad _ruleRd;

        std::map< int,BorderSeg > _borderSegs;
        std::map< int,Border> _borders;

        bool _isRectLike;
        int _idRefSeg; //reference for calculating construction trips (mainly for rectangle like parcel)
        Vector_2 _normal;

        std::map< RuleType, Rule*> _rules;

};


#endif // LOT_HPP
