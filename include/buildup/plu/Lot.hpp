#ifndef LOT_HPP
#define LOT_HPP

#include "geometry/geometry.hpp"
#include "Rule.hpp"
#include <gdal/ogrsf_frmts.h>
#include <map>
#include <string>
#include <boost/lexical_cast.hpp>

#ifndef M_PI
const double M_PI = 4.0 * atan(1.0);
#endif // #ifndef M_PI


enum class BorderType{
    Front,
    Side,
    Back,
    Corner,
    Unknown
};

class BorderSeg{
public:
    typedef geometry::Simple_cartesian<double> K;
    typedef typename K::Point_2 Point_2;
    typedef typename K::Segment_2 Segment_2;

    BorderSeg(int id_lot,int id,Segment_2& seg,BorderType t=BorderType::Unknown,int id_border=-1)
    : _id_lot(id_lot),_id(id),_seg(seg),_type(t),_id_border(id_border) {}

    inline int getID()          const {return _id;}
    inline Segment_2& getGeom()  {return _seg;}
    inline BorderType getType() const {return _type;}
    inline int getBorderID()    const {return _id_border;}

    inline void setGeom(Segment_2& seg){_seg=seg;}
    inline void setType(BorderType t){_type=t;}
    inline void setBorderID(int i){_id_border=i;}

    inline double squared_dist(const Point_2& p){
        //A,B: endpoints of segment, C: perpendicular foot from point to lineAB
        //r=product(AP,AB)/ |AB||AB|, vec_AC = r*vec_AB
        //if r<=0 dist=|AP|
        //if r>=1 dist=|BP|
        //else dist=|CP|
        double x = p.x(),y = p.y();
        double x1 = _seg.source().x(),y1 = _seg.source().y();
        double x2 = _seg.target().x(),y2 = _seg.target().y();

        double dot = (x-x1)*(x2-x1) + (y-y1)*(y2-y1);
        if(dot<=0)
            return (x-x1)*(x-x1) + (y-y1)*(y-y1);

        double sqLen =  (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
        if(dot>=sqLen)
            return (x-x2)*(x-x2) + (y-y2)*(y-y2);

        double cx = x1+(x2-x1)*dot/sqLen;
        double cy = y1+(y2-y1)*dot/sqLen;
        return (x-cx)*(x-cx)+(y-cy)*(y-cy);
    }

private:
    int _id_lot;
    int _id; //unique id in each lot
    Segment_2 _seg;
    BorderType _type;
    int _id_border; //belongs to which border
};

class Border{
public:
    typedef geometry::Simple_cartesian<double> K;
    typedef typename K::Segment_2 Segment_2;

    Border(int id_lot,int id,BorderType type,const char* name)
    :_id_lot(id_lot),_id(id),_type(type),_typeName(name) {}

    inline void addSeg(BorderSeg* pSeg){_pSegs.push_back(pSeg);}

    inline std::vector<BorderSeg*>& getSegs()   {return _pSegs;}
    inline Segment_2& getSeg(int i)             const {return _pSegs.at(i)->getGeom();}
    inline int getID()                          const {return _id;}
    inline BorderType getType()                 const {return _type;}
    inline std::string getTypeName()            const {return _typeName;}

private:
    int _id_lot;
    int _id; //unique id in each lot
    BorderType _type;
    std::string _typeName;
    std::vector<BorderSeg*> _pSegs;

};

class Lot
{
public:
    typedef geometry::Simple_cartesian<double> K;
    typedef typename K::Point_2 Point_2;
    typedef typename K::Vector_2 Vector_2;
    typedef typename K::Segment_2 Segment_2;

    Lot():_polygon(0) {}
    Lot(int id, OGRPolygon* ply): _id(id),_polygon((OGRPolygon*)(ply->clone())),_area(ply->get_Area())
    ,_idRefSeg(-1),_thetaRefSeg(-1),_translatedX(0.),_translatedY(0.){_polygon->getEnvelope(&_box);}

    //be careful, not all memebers are copied; currently only called by std::map insert
    Lot(const Lot& o){
        _id = o._id;
        _polygon = (OGRPolygon*)(o._polygon->clone());
        _area = o._area;
        _box = o._box;
        _idRefSeg = o._idRefSeg;
        _thetaRefSeg = o._thetaRefSeg;
        _translatedX = o._translatedX;
        _translatedY = o._translatedY;
    }
    ~Lot(){
        if(_polygon) _polygon->empty();
        std::map< RuleType, RuleEnergy*>::iterator it;
        for(it=_ruleEnergy.begin(); it!=_ruleEnergy.end(); ++it)
            if( it->second) delete it->second;
    }

    //manipulations
    void translate(double x0,double y0){
        _translatedX += x0;
        _translatedY += y0;
        //1) lot polygon
        //2) lot envelope
        //3) border segments
        int n=_polygon->getExteriorRing()->getNumPoints();
        for(int i=0; i<n; ++i)
        {
            double x = _polygon->getExteriorRing()->getX(i)+x0;
            double y = _polygon->getExteriorRing()->getY(i)+y0;
            _polygon->getExteriorRing()->setPoint(i,x,y);
        }

        _polygon->getEnvelope(&_box);

        Vector_2 pt0(x0,y0);
        std::map< int,BorderSeg>::iterator it;
        for(it=_borderSegs.begin(); it!=_borderSegs.end(); ++it)
        {
            Segment_2 seg(it->second.getGeom().source()+pt0,it->second.getGeom().target()+pt0);
            it->second.setGeom(seg);
        }
    }


    //predicates
    inline bool hasBorder(int id) const{return _borders.find(id)!=_borders.end();}
    inline bool hasRule(RuleType rule){return _ruleEnergy[rule]!=0;}

    //gets
    inline double id() const{return _id;}
    inline OGRPolygon* polygon() const{return _polygon;}
    inline double area() const{return _area;}
    inline double xMin() const{return _box.MinX;}
    inline double xMax() const{return _box.MaxX;}
    inline double yMin() const{return _box.MinY;}
    inline double yMax() const{return _box.MaxY;}
    inline RuleEnergy* ruleEnergy(RuleType t){return _ruleEnergy[t];}
    inline RuleGeom* ruleGeom() const{return _ruleGeom;}
    inline double translatedX() const{return _translatedX;}
    inline double translatedY() const{return _translatedY;}
    inline double invTransX() const{return -_translatedX;}
    inline double invTransY() const{return -_translatedY;}
    inline double thetaRef() const{return _thetaRefSeg;}

    //sets
    inline void insert_borderSeg(int id,BorderSeg& seg){_borderSegs.insert(std::make_pair(id,seg));}
    inline void insert_border(int id,Border& border){_borders.insert(std::make_pair(id,border));}
    inline void add_seg2border(int idSeg, int idBorder){(_borders.find(idBorder)->second).addSeg(&(_borderSegs.find(idSeg)->second));}
    void set_name_borders(){
        std::map<std::string, std::vector<Border*> > typeName_borders;

        std::map< int,Border>::iterator it;
        for(it=_borders.begin(); it!=_borders.end(); ++it)
            typeName_borders[it->second.getTypeName()].push_back(&(it->second));

        std::map<std::string, std::vector<Border*> >::iterator iter;
        for(iter=typeName_borders.begin(); iter!=typeName_borders.end(); ++iter)
        {
            int n = iter->second.size();
            if(n==1)
                _name_borders[iter->first] = (iter->second).at(0);

            else
            {
                for(int i=0; i<n; ++i)
                {
                    std::string name = iter->first;
                    name += boost::lexical_cast<std::string>(i+1);
                    _name_borders[name] = (iter->second).at(i);
                }

            }
        }
    }
    inline void insert_ruleEnergy(RuleType t, RuleEnergy* r){_ruleEnergy[t] = r;}
    inline void set_ruleGeom(RuleGeom* r){_ruleGeom=r;}
    void set_isRectLike(bool isRectLike){
        if(isRectLike)
        {
            std::map<int,BorderSeg>::iterator it;
            for(it=_borderSegs.begin(); it!=_borderSegs.end(); ++it)
                if(it->second.getType()==BorderType::Front)
                {
                    //reference to the first front border segment
                    _idRefSeg = it->second.getID();
                    Segment_2 refSeg = _borderSegs.find(_idRefSeg)->second.getGeom();
                    Vector_2 vec = refSeg.target()-refSeg.source();
                    _thetaRefSeg = std::atan2(vec.y(),vec.x()); //(-pi,pi]
                    if(_thetaRefSeg<0)
                        _thetaRefSeg += M_PI;
                    return;
                }
        }
    }


    //computation
    double refTheta(double x,double y){
        //Theta of its nearest border segment
        if(_idRefSeg>-1)
            return _thetaRefSeg;

        Point_2 center(x,y);

        std::map< int,BorderSeg >::iterator iter = _borderSegs.begin();
        double d2min = iter->second.squared_dist(center);
        int idNearest = iter->first;
        iter++;
        for(; iter != _borderSegs.end(); ++iter)
        {
            double d2 = iter->second.squared_dist(center);
            if(d2<d2min)
            {
                d2min = d2;
                idNearest = iter->first;
            }
        }

        Segment_2& s = _borderSegs.find(idNearest)->second.getGeom();
        Vector_2 vec = s.target() - s.source();
        double theta = std::atan2(vec.y(),vec.x());
        if(theta<0)
            theta += M_PI;
        return theta;
    }

    double refTheta_front(double x,double y){
        if(_idRefSeg>-1)
            return _thetaRefSeg;


        std::map< int,BorderSeg >::iterator iter = _borderSegs.begin();
        while(iter!=_borderSegs.end() && iter->second.getType()!=BorderType::Front)
            iter++;

        Point_2 center(x,y);
        double d2min = iter->second.squared_dist(center);
        int idNearest = iter->first;
        iter++;

        for(; iter != _borderSegs.end(); ++iter)
        {
            if(iter->second.getType()!=BorderType::Front)
                continue;

            double d2 = iter->second.squared_dist(center);
            if(d2<=d2min)
            {
                d2min = d2;
                idNearest = iter->first;
            }
        }

        Segment_2& s = _borderSegs.find(idNearest)->second.getGeom();
        Vector_2 vec = s.target() - s.source();
        double theta = std::atan2(vec.y(),vec.x());
        if(theta<0)
            theta += M_PI;
        return theta;
    }

    template<typename OBJ>
    void dist2borders(OBJ& obj,std::map< std::string,double >& result){
        std::map<std::string,Border*>::iterator it;

        for(it=_name_borders.begin(); it!=_name_borders.end(); ++it)
        {
            std::vector<BorderSeg*>& segs = it->second->getSegs();
            int n = segs.size();
            if(n<1)
            {
                std::cerr<<"empty border ";
                exit(1);
            }
            double d2min = segs.at(0)->squared_dist(obj.bottom().center());
            int idNearest = segs.at(0)->getID();
            for(int i=1; i<n; ++i)
            {
                double d2 = segs.at(i)->squared_dist(obj.bottom().center());
                if(d2<d2min)
                {
                    d2min=d2;
                    idNearest=segs.at(i)->getID();
                }
            }

            double dist = obj.distance2line(_borderSegs.find(idNearest)->second.getGeom());
            result[it->first]= dist;
        }
    }


    //only used in io::save_borderSegs2shp for manually editing border type
    void extractBorderSegs(std::map< int,BorderSeg >& borderSegs){
        int idSeg = -1;
        OGRLinearRing* ring = _polygon->getExteriorRing();
        for(int i=0; i<ring->getNumPoints()-1; ++i)
        {
            Point_2 pt1(ring->getX(i),ring->getY(i));
            Point_2 pt2(ring->getX(i+1),ring->getY(i+1));
            ++idSeg;
            Segment_2 s(pt1,pt2);
            borderSegs.insert(std::make_pair(idSeg,BorderSeg(_id,idSeg,s)));
        }

        for(int j=0; j<_polygon->getNumInteriorRings(); ++j)
        {
            ring = _polygon->getInteriorRing(j);
            for(int i=0; i<ring->getNumPoints()-1; ++i)
            {
                Point_2 pt1(ring->getX(i),ring->getY(i));
                Point_2 pt2(ring->getX(i+1),ring->getY(i+1));
                ++idSeg;
                Segment_2 s(pt1,pt2);
                borderSegs.insert(std::make_pair(idSeg,BorderSeg(_id,idSeg,s)));

            }
        }
    }

private:
    int _id;
    OGRPolygon* _polygon;
    double _area;
    OGREnvelope _box;

    std::map< int,BorderSeg > _borderSegs;
    std::map< int,Border> _borders;
    std::map< std::string,Border* > _name_borders;

    std::map< RuleType, RuleEnergy*> _ruleEnergy;
    RuleGeom* _ruleGeom;

    int _idRefSeg;
    double _thetaRefSeg;

    double _translatedX;
    double _translatedY;

};


#endif // LOT_HPP
