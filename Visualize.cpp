#include "Visualize.hpp"
#include <osgUtil/Tessellator>

Visualize::Visualize()
{
    //ctor
}

Visualize::~Visualize()
{
    //dtor
}

void Visualize::display(Geometry& geometry, osg::Vec4 geomColor, osg::Vec4 camColor)
{
    viewNode(makeGeode(geometry, geomColor),camColor);
}

void Visualize::display(std::vector<Bldg>& bldgs, Lot* lot, osg::Vec4 geomColor, osg::Vec4 camColor)
{
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(makeGeode(lot->_polygon,osg::PrimitiveSet::Mode::POLYGON,true,YELLOWLIGHT));
    for(int i=0;i<bldgs.size();++i)
    {
        if(bldgs[i]._block.isEmpty())
            bldgs[i].extrude();
        root->addChild(makeGeode(bldgs[i]._block,geomColor));
    }
    viewNode(root,camColor);

}

/**********************************************************************
private functions *****************************************************
***********************************************************************/
osg::ref_ptr<osg::Geode> Visualize::makeGeode(Geometry& geometry, osg::Vec4 color)
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

void Visualize::viewNode(osg::ref_ptr<osg::Node> node, osg::Vec4 camColor)
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

    viewer.getCamera()->setClearColor(camColor);
	viewer.setSceneData(node.get());
	viewer.realize();
	viewer.run();
}


osg::ref_ptr<osg::Geode> Visualize::makeGeode(OGRPolygon* polygon, osg::PrimitiveSet::Mode mode,bool doTessellate, osg::Vec4 color)
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

	return geode;
}
