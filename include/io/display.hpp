#ifndef IO_OSG_HPP
#define IO_OSG_HPP

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osg/LightModel>
#include <osg/LineWidth>
#include <osgText/Text>


#include "sfcgal.hpp"
#include <gdal/ogrsf_frmts.h>
#include <vector>
#include <map>
#include "Lot.hpp"
#include "Building.hpp"
#include "io/convert.hpp"
#include <osgUtil/Tessellator>
#include <SFCGAL/io/OsgFactory.h>
#include <SFCGAL/algorithm/offset.h>
#include <string>
#include <boost/lexical_cast.hpp>
namespace io
{
    #define WHITE osg::Vec4f(1.0f,1.0f,1.0f,0.2f)
    #define BLACK osg::Vec4f(0.0f,0.0f,0.0f,1.0f)

    #define YELLOW osg::Vec4f(1.0f,1.0f,0.0f,1.0f)
    #define YELLOWLIGHT osg::Vec4f(1.0f,0.92f,0.54f,1.0f)
    #define LIGHTGRAY osg::Vec4f(0.83f,0.83f,0.83f,0.2f)

    #define RED osg::Vec4f(1.0f,0.0f,0.0f,1.0f)
//    #define CORAL osg::Vec4f(1.0f,0.50f,0.31f,0.4f)
//    #define GREEN osg::Vec4f(0.07f,0.22f,0.13f,0.4f)

    #define AZURE2 osg::Vec4f(0.88f,0.93f,0.93f,1.0f)
//    #define LIGHTGREEN osg::Vec4f(0.62f,0.67f,0.48f,1.0f)
//    #define LIGHTBLUE osg::Vec4f(0.68f,0.84f,0.90f,1.0f)
    #define SKYBLUE3 osg::Vec4f(0.42f,0.65f,0.80f,1.0f)
//    #define DEEPSKYBLUE4 osg::Vec4f(0.0f,0.40f,0.55f,1.0f)


    osg::ref_ptr<osg::Geode> makeGeode(Geometry& geometry, osg::Vec4 color);
    int viewNode(osg::ref_ptr<osg::Node> node);
    osg::ref_ptr<osg::Geode> makeGeode(OGRPolygon* polygon, osg::PrimitiveSet::Mode mode,bool doTessellate, osg::Vec4 color);



    void display(Geometry& geometry, osg::Vec4 geomColor)
    {
        viewNode(makeGeode(geometry, geomColor));
    }

    void display(std::vector<Bldg>& bldgs, Lot* lot)
    {
        osg::ref_ptr<osg::Group> root = new osg::Group();
        root->addChild(makeGeode(lot->polygon(),osg::PrimitiveSet::Mode::POLYGON,true,YELLOWLIGHT));

    //    //main edge (front border)
    //    Point p1(lot->_mainEdge.source().x(),lot->_mainEdge.source().y());
    //    Point p2(lot->_mainEdge.target().x(),lot->_mainEdge.target().y());
    //    LineString s(p1,p2);
    //    root->addChild(makeGeode(s,BLACK));

        for(size_t i=0;i<bldgs.size();++i)
        {
            if(bldgs[i]._block.isEmpty())
                bldgs[i].extrude();
            //root->addChild(makeGeode(bldgs[i]._block,geomColor));

            osg::ref_ptr<osg::Group> wire = new osg::Group();
            PolyhedralSurface shell = bldgs[i]._block.exteriorShell();
            int n=shell.numPolygons();
            for(int j=0;j<n;++j)
            {
                wire->addChild(makeGeode(shell.polygonN(j).exteriorRing(),BLACK));
            }
            root->addChild(wire);

        }
        viewNode(root);

    }

    void display(std::vector<Bldg>& bldgs, std::map<int,Lot>& lots)
    {
        osg::ref_ptr<osg::Group> root = new osg::Group();


        for(size_t i=0;i<bldgs.size();++i)
        {
            if(bldgs[i]._block.isEmpty())
                bldgs[i].extrude();

            osg::ref_ptr<osg::Group> wire = new osg::Group();
            PolyhedralSurface shell = bldgs[i]._block.exteriorShell();
            int n=shell.numPolygons();
            for(int j=0;j<n;++j)
            {
                wire->addChild(makeGeode(shell.polygonN(j).exteriorRing(),RED));
            }
            root->addChild(wire);

        }

        std::map<int,Lot>::iterator it;
        for(it=lots.begin();it!=lots.end();++it)
        {
            OGRPolygon *ply = (OGRPolygon*)(it->second.polygon()->Buffer(-0.1));
            root->addChild(makeGeode(ply,osg::PrimitiveSet::Mode::POLYGON,true,WHITE));
            ply->empty();
            root->addChild(makeGeode(it->second.polygon(),osg::PrimitiveSet::Mode::LINE_LOOP,false,BLACK));
        }

        viewNode(root);
    }


//    void display(std::vector<Bldg>& bldgs, std::map<int,Lot>& lots,std::map<int,Road>&roads)
//    {
//        osg::ref_ptr<osg::Group> root = new osg::Group();
//
//
//        for(size_t i=0;i<bldgs.size();++i)
//        {
//            if(bldgs[i]._block.isEmpty())
//                bldgs[i].extrude();
//
//            //MultiPolygon multiPly = *(SFCGAL::algorithm::offset(bldgs[i]._block,-1.0).release());
//            //root->addChild(makeGeode(bldgs[i]._block,geomColor));
//
//            osg::ref_ptr<osg::Group> wire = new osg::Group();
//            osg::ref_ptr<osg::Group> surface = new osg::Group();
//            PolyhedralSurface shell = bldgs[i]._block.exteriorShell();
//            int n=shell.numPolygons();
//            for(int j=0;j<n;++j)
//            {
//                wire->addChild(makeGeode(shell.polygonN(j).exteriorRing(),RED));
//                surface->addChild(makeGeode(shell.polygonN(j),LIGHTGRAY));
//            }
//            //root->addChild(wire);
//            root->addChild(surface);
//        }
//
//        std::map<int,Lot>::iterator it;
//        for(it=lots.begin();it!=lots.end();++it)
//        {
////            Polygon ply = OGR2SFCGAL(it->second._polygon);
////            root->addChild(makeGeode(*(SFCGAL::algorithm::offset(ply,-1.0).release()),YELLOWLIGHT));
//            OGRPolygon *ply = (OGRPolygon*)(it->second.polygon()->Buffer(-0.1));
//            root->addChild(makeGeode(ply,osg::PrimitiveSet::Mode::POLYGON,true,WHITE));
//            ply->empty();
//            root->addChild(makeGeode(it->second.polygon(),osg::PrimitiveSet::Mode::LINE_LOOP,false,BLACK));
//        }
//
//        std::map<int,Road>::iterator itRd;
//        for(itRd=roads.begin();itRd!=roads.end();++itRd)
//        {
//            OGRPolygon *ply = (OGRPolygon*)(itRd->second._polyline->Buffer(itRd->second._width/2,4));
//            root->addChild(makeGeode(ply,osg::PrimitiveSet::Mode::POLYGON,true,GREEN));
//        }
//
//        viewNode(root);
//    }

    void display(std::map<int,std::vector<Bldg> >& exp_bldgs, std::map<int,Lot>& lots)
    {
        osg::ref_ptr<osg::Group> root = new osg::Group();

        {
            std::map<int,std::vector<Bldg> >::iterator it;
            for(it=exp_bldgs.begin();it!=exp_bldgs.end();++it)
            {
                osg::ref_ptr<osg::Group> exp = new osg::Group();
                std::vector<Bldg>& bldgs = it->second;
                for(size_t i=0;i<bldgs.size();++i)
                {
                    if(bldgs[i]._block.isEmpty())
                        bldgs[i].extrude();

                    osg::ref_ptr<osg::Group> wire = new osg::Group();
                    osg::ref_ptr<osg::Group> surface = new osg::Group();
                    PolyhedralSurface shell = bldgs[i]._block.exteriorShell();
                    int n=shell.numPolygons();
                    for(int j=0;j<n;++j)
                    {
                        wire->addChild(makeGeode(shell.polygonN(j).exteriorRing(),BLACK));
                        surface->addChild(makeGeode(shell.polygonN(j),SKYBLUE3));
                    }
                    exp->addChild(wire);
                    exp->addChild(surface);

//                    osg::ref_ptr<osgText::Text> text = new osgText::Text;
//                    osg::ref_ptr<osg::Geode>label = new osg::Geode;
//                    text->setPosition(osg::Vec3(50,50,50));
//					   text->setAlignment(osgText::Text::CENTER_BOTTOM);
//					   text->setAxisAlignment(osgText::Text::XZ_PLANE);
//                    std::string s("Experiment ");
//                    s += boost::lexical_cast<std::string>(i);
//                    text->setText(s.c_str());
//                    label->addDrawable(text);
//                    exp->addChild(label);

                }



                exp->setNodeMask(0);
                root->addChild(exp);
            }
            root->getChild(0)->setNodeMask(1);
        }

        {
            osg::ref_ptr<osg::Group> gpLots = new osg::Group();
            std::map<int,Lot>::iterator it;
            for(it=lots.begin();it!=lots.end();++it)
            {
                OGRPolygon *ply = (OGRPolygon*)(it->second.polygon()->Buffer(-0.1));
                gpLots->addChild(makeGeode(ply,osg::PrimitiveSet::Mode::POLYGON,true,WHITE));
                ply->empty();
                gpLots->addChild(makeGeode(it->second.polygon(),osg::PrimitiveSet::Mode::LINE_LOOP,false,BLACK));
            }
            root->addChild(gpLots);
        }
        viewNode(root);
    }


    //void displayMultiView(std::vector<Bldg>& bldgs,std::map<int,Lot>& lots,std::map<int,Road>&roads )

   void display(std::map<int,std::vector<Bldg> >& i_bldgs)
    {
        osg::ref_ptr<osg::Group> root = new osg::Group();

        {
            std::map<int,std::vector<Bldg> >::iterator it;
            for(it=i_bldgs.begin();it!=i_bldgs.end();++it)
            {
                osg::ref_ptr<osg::Group> gp_i = new osg::Group();
                std::vector<Bldg>& bldgs = it->second;
                for(size_t i=0;i<bldgs.size();++i)
                {
                    if(bldgs[i]._block.isEmpty())
                        bldgs[i].extrude();

                    osg::ref_ptr<osg::Group> wire = new osg::Group();
                    osg::ref_ptr<osg::Group> surface = new osg::Group();
                    PolyhedralSurface shell = bldgs[i]._block.exteriorShell();
                    int n=shell.numPolygons();
                    for(int j=0;j<n;++j)
                    {
                        wire->addChild(makeGeode(shell.polygonN(j).exteriorRing(),BLACK));
                        surface->addChild(makeGeode(shell.polygonN(j),SKYBLUE3));
                    }
                    gp_i->addChild(wire);
                    gp_i->addChild(surface);

//                    osg::ref_ptr<osgText::Text> text = new osgText::Text;
//                    osg::ref_ptr<osg::Geode>label = new osg::Geode;
//                    text->setPosition(osg::Vec3(50,50,50));
//					   text->setAlignment(osgText::Text::CENTER_BOTTOM);
//					   text->setAxisAlignment(osgText::Text::XZ_PLANE);
//                    std::string s("Experiment ");
//                    s += boost::lexical_cast<std::string>(i);
//                    text->setText(s.c_str());
//                    label->addDrawable(text);
//                    exp->addChild(label);

                }



                gp_i->setNodeMask(0);
                root->addChild(gp_i);
            }
            root->getChild(0)->setNodeMask(1);
        }
        viewNode(root);
    }

    void display(OGRMultiPolygon* plys)
    {
        osg::ref_ptr<osg::Group> root = new osg::Group();
        int n=plys->getNumGeometries();
        for(int i=0;i<n;++i)
            root->addChild(makeGeode((OGRPolygon*)(plys->getGeometryRef(i)),osg::PrimitiveSet::Mode::LINE_STRIP,false,BLACK));
        viewNode(root);
    }
///////////////////////////////////////////////////////////////////////////////////////////////
        class UseEventHandler:public osgGA::GUIEventHandler
    {

    public:
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
        {
            osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
            if(!viewer)return false;

            if (ea.getEventType()==osgGA::GUIEventAdapter::KEYDOWN)
            {

                int n = viewer->getSceneData()->asGroup()->getNumChildren()-1;  // number of layers (footprints/lod models)

                if(ea.getKey()==osgGA::GUIEventAdapter::KEY_Up)
                    for(int i= 0;i<n;i++)
                        if(viewer->getSceneData()->asGroup()->getChild(i)->getNodeMask()&& i+1<n)
                        {
                            viewer->getSceneData()->asGroup()->getChild(i+1)->setNodeMask(1);
                            viewer->getSceneData()->asGroup()->getChild(i)->setNodeMask(0);
                            break;

                        }

                if(ea.getKey()==osgGA::GUIEventAdapter::KEY_Down)
                    for(int i=n-1;i>=0;i--)
                        if(viewer->getSceneData()->asGroup()->getChild(i)->getNodeMask() && i-1>=0)
                        {
                            viewer->getSceneData()->asGroup()->getChild(i)->setNodeMask(0);
                            viewer->getSceneData()->asGroup()->getChild(i-1)->setNodeMask(1);
                            break;

                        }

            }
            return false;
        }
    };




    /**********************************************************************
    not interface functions *****************************************************
    ***********************************************************************/
    osg::ref_ptr<osg::Geode> makeGeode(Geometry& geometry, osg::Vec4 color)
    {
        SFCGAL::io::OsgFactory factory;

        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        osg::ref_ptr<osg::Geometry> geom = factory.createGeometry(geometry);

        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        colors->push_back(color);
        geom->setColorArray(colors);
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geode->addDrawable(geom);

    //	//blend
    //	osg::StateSet* ss = geode->getOrCreateStateSet();
    //	ss->setMode(GL_BLEND,osg::StateAttribute::ON);
    //	ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

        return geode;
    }

    int viewNode(osg::ref_ptr<osg::Node> node)
    {
        osgViewer::Viewer viewer;
        viewer.setLightingMode(osg::View::NO_LIGHT);
        osg::StateSet* globalState = viewer.getCamera()->getStateSet();
        if(globalState)
        {
            osg::LightModel* lightModel = new osg::LightModel;
            lightModel->setAmbientIntensity(osg::Vec4(0.3,0.3,0.3,1));
            globalState->setAttributeAndModes(lightModel,osg::StateAttribute::ON);
        }
        viewer.setUpViewInWindow(0,0,600,400);
        viewer.setCameraManipulator(0);
        viewer.getCamera()->setClearColor(AZURE2);
        viewer.setSceneData(node.get());
        viewer.addEventHandler(new UseEventHandler());
        viewer.realize();
        viewer.run();
//        osg::Vec3 eye,center,up;
//        int i=0;
//        while(!(viewer.done()))
//        {
//            viewer.frame();
//            if(i%50==0)
//            {
//            viewer.getCamera()->getViewMatrixAsLookAt(eye,center,up);
//            std::cout<<i<<"\n";
//            std::cout<<"eye "<<eye.x()<<" "<<eye.y()<<" "<<eye.z()<<"\n";
//            std::cout<<"center "<<center.x()<<" "<<center.y()<<" "<<center.z()<<"\n";
//            std::cout<<"up "<<up.x()<<" "<<up.y()<<" "<<up.z()<<"\n";
//            }
//            i++;
//        }
        return 0;
    }

    osg::ref_ptr<osg::Geode> makeGeode(OGRPolygon* polygon, osg::PrimitiveSet::Mode mode,bool doTessellate, osg::Vec4 color)
    {
        if(!polygon || polygon->IsEmpty())
            return 0;
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        osg::Geometry *geom = new osg::Geometry();
        osg::Vec3Array *verts = new osg::Vec3Array();
        geom->setVertexArray(verts);

        OGRLinearRing *ring = polygon->getExteriorRing();

        int n_pts = ring->getNumPoints()-1;

        for(int j=n_pts-1;j>-1;j--)
        {
            OGRPoint pt;
            ring->getPoint(j,&pt);
            verts->push_back(osg::Vec3(pt.getX(),pt.getY(),pt.getZ()));
        }
        int n_start = 0;
        geom->addPrimitiveSet(new osg::DrawArrays(mode,n_start,n_pts));
        n_start += n_pts;

        if(int n_rings_inter = polygon->getNumInteriorRings())
        {	for(int j=0;j<n_rings_inter;j++)
            {
                OGRLinearRing *ring_j = polygon->getInteriorRing(j);

                int n_pts_j = ring_j->getNumPoints()-1;

                for(int k=n_pts_j-1;k>-1;k--)
                {
                    OGRPoint pt;
                    ring_j->getPoint(k,&pt);
                    verts->push_back(osg::Vec3(pt.getX(),pt.getY(),pt.getZ()));
                }
                geom->addPrimitiveSet(new osg::DrawArrays(mode,n_start,n_pts_j));
                n_start += n_pts_j;

            }
        }

        if(mode==osg::PrimitiveSet::POLYGON && doTessellate)
        {
            osg::ref_ptr<osgUtil::Tessellator> tess = new osgUtil::Tessellator;
            tess->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
            tess->setBoundaryOnly(false);
            tess->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
            tess->retessellatePolygons(*geom);

        }



        osg::Vec4Array* colors = new osg::Vec4Array();
        colors->push_back(color);
        geom->setColorArray(colors);
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);
        geom->getTexCoordArrayList().clear();

        geode->addDrawable(geom);

        osg::LineWidth* width = new osg::LineWidth;
        width->setWidth(2.0);
        geode->getOrCreateStateSet()->setAttributeAndModes(width,osg::StateAttribute::ON);



        return geode;
    }


}//namespace io
#endif // IO_OSG_HPP
