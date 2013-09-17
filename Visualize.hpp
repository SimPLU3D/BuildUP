#ifndef VISUALIZE_HPP
#define VISUALIZE_HPP

#include "sfcgalTypes.hpp"
#include <SFCGAL/io/OsgFactory.h>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osg/LightModel>

#define WHITE osg::Vec4f(1.0f,1.0f,1.0f,0.2f)
#define BLACK osg::Vec4f(0.0f,0.0f,0.0f,1.0f)

#define YELLOW osg::Vec4f(1.0f,1.0f,0.0f,1.0f)
#define YELLOWLIGHT osg::Vec4f(1.0f,0.92f,0.54f,1.0f)
#define LIGHTGRAY osg::Vec4f(0.83f,0.83f,0.83f,0.2f)

#define RED osg::Vec4f(1.0f,0.0f,0.0f,0.4f)
#define CORAL osg::Vec4f(1.0f,0.50f,0.31f,0.4f)
#define GREEN osg::Vec4f(0.07f,0.22f,0.13f,0.4f)

#define AZURE2 osg::Vec4f(0.88f,0.93f,0.93f,1.0f)
#define LIGHTGREEN osg::Vec4f(0.62f,0.67f,0.48f,1.0f)
#define LIGHTBLUE osg::Vec4f(0.68f,0.84f,0.90f,1.0f)
#define SKYBLUE3 osg::Vec4f(0.42f,0.65f,0.80f,1.0f)
#define DEEPSKYBLUE4 osg::Vec4f(0.0f,0.40f,0.55f,1.0f)

#include <vector>
#include "plu/Lot.hpp"
#include "plu/Bldg.hpp"
#include <gdal/ogrsf_frmts.h>

class Visualize
{
    public:
        Visualize();
        ~Visualize();

        void display(Geometry&, osg::Vec4 geomColor, osg::Vec4 camColor);
        void display(std::vector<Bldg>&, Lot*, osg::Vec4 geomColor, osg::Vec4 camColor);

    private:
        osg::ref_ptr<osg::Geode> makeGeode(Geometry&, osg::Vec4 geomColor);
        osg::ref_ptr<osg::Geode> makeGeode(OGRPolygon*, osg::PrimitiveSet::Mode,bool doTessellate, osg::Vec4 geomColor);
        void viewNode(osg::ref_ptr<osg::Node> node, osg::Vec4 camColor);
};

#endif // VISUALIZE_HPP
