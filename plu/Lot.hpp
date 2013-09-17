#ifndef LOT_HPP
#define LOT_HPP

#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "Rule.hpp"
//#include <SFCGAL/all.h>
#include <gdal/ogrsf_frmts.h>


//typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;

class Lot{
    public:
        typedef geometry::Simple_cartesian<double> K;
        typedef typename geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;
        typedef typename K::Point_2 Point_2;
        typedef typename K::Vector_2 Vector_2;
        typedef typename K::Segment_2 Segment_2;

        Lot(OGRPolygon* ply, RuleLot& rule,Iso_rectangle_2& box)
        : _polygon((OGRPolygon*)(ply->clone())),_area(ply->get_Area())
        , _rule(rule), _box2d(box)
        //find the longest edge as the main edge
        {set_mainEdge_longest();}

//        Lot(OGRPolygon* ply, RuleLot& rule, RuleRoad& ruleRd,Iso_rectangle_2& box)
//        : _polygon((OGRPolygon*)(ply->clone())),_area(ply->get_Area())
//        , _rule(rule),_ruleRd(ruleRd), _box2d(box)
//        //find the longest edge as the main edge
//        {set_mainEdge_longest();}

//         Lot(OGRPolygon* ply, RuleLot& rule, Iso_rectangle_2& box,Segment_2& mainEdge)
//        : _polygon((OGRPolygon*)(ply->clone())), _rule(rule),_box2d(box),_area(ply->get_Area())
//        ,_mainEdge(mainEdge),_normal(_mainEdge.target()-_mainEdge.source()){}

        ~Lot(){_polygon->empty();}

        Vector_2 getBoxNormal()
        {
            double dx=_box2d.max().x()-_box2d.min().x();
            double dy=_box2d.max().y()-_box2d.min().y();
            return dx>dy? Vector_2(dx/2,0):Vector_2(0,dy/2);
        }


    private:
        void set_mainEdge_longest()
        {
            _polygon->closeRings();
            OGRLinearRing* ring = _polygon->getExteriorRing();
            int n=ring->getNumPoints();
            std::cout<<"number of points"<<n<<std::endl;
            if(n<4)
                return;
            OGRPoint pt[n];
            for(int i=0;i<n;i++)
                ring->getPoint(i,&pt[i]);
            double d[n-1],dmax=0;
            int dmax_id = -1;
            for(int i=0;i<n-1;i++)
            {
                d[i]=pt[i].Distance(&pt[i+1]);
                dmax_id = d[i]>dmax? i:dmax_id;
                dmax = d[i]>dmax? d[i]:dmax;
            }
            Point_2 pt1(pt[dmax_id].getX(),pt[dmax_id].getY());
            Point_2 pt2(pt[dmax_id+1].getX(),pt[dmax_id+1].getY());
            std::cout<<pt1.x()<<" "<<pt1.y()<<std::endl;
            std::cout<<pt2.x()<<" "<<pt2.y()<<std::endl;
            _mainEdge =  Segment_2(pt1,pt2);
            _normal = _mainEdge.target()-_mainEdge.source();
        }

    public:
        OGRPolygon* _polygon;
        double _area;
        RuleLot _rule;
        RuleRoad _ruleRd;
        Iso_rectangle_2 _box2d;

        Segment_2 _mainEdge;
        Vector_2 _normal;
        //std::vector<Road*> _roads;



};


#endif // LOT_HPP
