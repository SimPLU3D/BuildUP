
/************ geometry ******************/
#include "geometry/geometry.hpp"
#include "geometry/Rectangle_2.hpp"
#include "geometry/Cuboid_bldg.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
//typedef K::Vector_2 Vector_2;
//typedef K::Segment_2 Segment_2;
typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2; //for bounding box
typedef geometry::Rectangle_2<K> Rectangle_2;
typedef geometry::Cuboid_bldg<K> Cuboid_bldg;
typedef Cuboid_bldg object;

#include "geometry/coordinates/Rectangle_2_coordinates.hpp"
#include "geometry/intersection/Rectangle_2_intersection.hpp"

#include "geometry/coordinates/Cuboid_bldg_coordinates.hpp"
#include "geometry/intersection/Cuboid_bldg_intersection.hpp"


/************  energies  ******************/
#include "rjmcmc/energy/energy_operators.hpp"
#include "mpp/energy/plu_unary_energy.hpp"
#include "mpp/energy/plu_binary_energy.hpp"
typedef plu_unary_ces<> unary_ces;
typedef plu_unary_cos<> unary_cos;
typedef plu_unary_border<> unary_border;
typedef plu_binary_intersection<> binary_overlap;
typedef plu_binary_distance<> binary_distance;


/************ configuration ******************/
#include "mpp/configuration/graph_configuration.hpp"
typedef marked_point_process::graph_configuration<
        object
        ,multiplies_energy<constant_energy<>,unary_ces>
        ,multiplies_energy<constant_energy<>,unary_cos>
        ,multiplies_energy<constant_energy<>,unary_border>
        ,multiplies_energy<constant_energy<>,binary_overlap>
        ,multiplies_energy<constant_energy<>,binary_distance>
        > configuration;

/***** kernels *****/
#include "mpp/kernel/kernel.hpp"
typedef marked_point_process::result_of_make_uniform_birth_death_kernel<object>::type  birth_death_kernel;

#include "rjmcmc/kernel/transform.hpp"
#include "geometry/kernels/rectangle_rotation_scaled_corner_kernel.hpp"
#include "geometry/kernels/rectangle_scaled_edge_kernel.hpp"
#include "geometry/kernels/cuboid_transform_kernel.hpp"
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_edge_translation_transform  >::type  edge_modification_kernel;
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_corner_translation_transform>::type  corner_modification_kernel;
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_height_scaling_transform>::type  height_modification_kernel;


/***** smaplers *****/
//[building_footprint_rectangle_definition_distribution
#include "rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution distribution;
//]

#include "rjmcmc/sampler/sampler.hpp"
#include "plu/Filter.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
typedef marked_point_process::rejection_birth<uniform_birth,Filter> rejection_birth;

#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,rejection_birth> d_sampler;



#include "rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

//typedef rjmcmc::sampler<d_sampler,acceptance,birth_death_kernel> sampler;
typedef rjmcmc::sampler<d_sampler,acceptance
        ,birth_death_kernel
        ,edge_modification_kernel
        //,corner_modification_kernel
        ,height_modification_kernel
        > sampler;

/***** simulated_annealing *****/
/*< Choice of the schedule (/include/ and /typedef/) >*/
#include "simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;
/*< Choice of the end_test (/include/ and /typedef/) >*/
#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
typedef simulated_annealing::max_iteration_end_test     end_test;

#include "simulated_annealing/simulated_annealing.hpp"

#include "param/parameter.hpp"
#include "param/parameters_inc.hpp"

#include <gdal/ogrsf_frmts.h>
#include "plu/Lot.hpp"
#include "plu/Rule.hpp"
#include <vector>

#include "rjmcmc/sampler/any_sampler.hpp"
#include "simulated_annealing/visitor/any_visitor.hpp"
#include "simulated_annealing/visitor/ostream_visitor.hpp"
#include "simulated_annealing/visitor/shp/shp_visitor.hpp"
#include "simulated_annealing/visitor/tex_visitor.hpp"

#include "Visualize.hpp"
#include <iostream>
#include <fstream>
#include <math.h>
#include <boost/filesystem.hpp>
#include "plu/Bldg.hpp"
#include "plu/Road.hpp"

OGRPolygon* loadFromShp(const char* shpIn);
Lot* singleLotFromShp(const char* shpIn, const char* shpOut);

int main(int argc,char** argv)
{
    const char * shpIn = "./data/lot6.shp";
    const char * shpOut = "./data/lot6_trans.shp";
    Lot* lot = singleLotFromShp(shpIn,shpOut);
    std::cout<<"lot area"<<lot->_area<<std::endl;
//	std::cout<<lot->_polygon->IsEmpty()<<std::endl;

//	//test
//	OGRPolygon* ply = lot->_polygon;
//    OGRLinearRing* ring1 = ply->getExteriorRing();
//	std::cout<<"num of points "<<ring1->getNumPoints()<<std::endl;
//	for(int i=0;i<ring1->getNumPoints();i++)
//	{
//        std::cout<<std::fixed<<ring1->getX(i)<<" "<<ring1->getY(i)<<std::endl;
//	}
//

    typedef parameters< parameter > param;
    param *p = new param;

    p->caption("Building generation parameters");
    p-> insert<double>("temp",'t',1000,"Initial Temperature");
    p-> insert<double>("deccoef",'C',0.999999,"Decrease coefficient");
    p-> insert<int>("nbiter",'I',150000,"Number of iterations");
//    p->template insert<double>("qtemp",'q',0.5,"Sampler (q) [0;1]");
    p-> insert<int>("nbdump",'d',150000,"Number of iterations between each result display");
//    p->template insert<bool>("dosave",'b',false, "Save intermediate results");
    p-> insert<int>("nbsave",'S',150000,"Number of iterations between each save");
    p-> insert<double>("poisson",'p',200, "Poisson processus parameter");
    p-> insert<double>("maxsize",'Ms', lot->_box2d.area()*lot->_rule._cesMax, "Maximum rectangle area");
    p-> insert<double>("maxratio",'Mr',lot->_rule._ratioMax, "Maximum rectangle aspect ratio");
    p-> insert<double>("maxheight",'mh',lot->_rule._hMax, "Maximum height");
    p-> insert<double>("minheight",'Mh',lot->_rule._hMin, "Minimum height");

    p-> insert<double>("pbirth",'B',0.6, "Birth probability");
    p-> insert<double>("pdeath",'D',0.1, "Death probability");

    p-> insert<double>("pond_ces",'ces',300, "ces weight");
    p-> insert<double>("pond_cos",'cos',500, "cos weight");
    p-> insert<double>("pond_dist_ur",'distur',10,"unary distance weight");
    p-> insert<double>("pond_overlap",'overlap',50, "overlap weight");
    p-> insert<double>("pond_dist_bi",'distbi',10, "binary distance weight");

    p-> insert<double>("rej_energy_ces",'rjces',200, "rejection energy of ces");
    p-> insert<double>("rej_energy_cos",'rjcos',300, "rejection energy of cos");
    p-> insert<double>("rej_energy_dist_bi",'rjdistbi',200, "rejection energy of binary distance ");
    p-> insert<double>("rej_energy_dist_ur",'rjdistur',200, "rejection energy of unary distance ");




    /*** configuration ***/
    configuration conf( p->get<double>("pond_ces")*unary_ces(lot)
                        ,p->get<double>("pond_cos")*unary_cos(lot)
                        ,p->get<double>("pond_dist_ur")*unary_border(lot,p->get<double>("rej_energy_dist_ur"))
                        ,p->get<double>("pond_overlap")*binary_overlap()
                        ,p->get<double>("pond_dist_bi")*binary_distance(lot->_rule._distMinBi,p->get<double>("rej_energy_dist_bi"))
                        //,global_energy(lot)
                        );
    conf.setMaxCES(p->get<double>("pond_ces")*lot->_rule._cesMax);
    conf.setMaxCOS(p->get<double>("pond_cos")*lot->_rule._cosMax);
    conf.set_rejectionEnergy_ces(p->get<double>("pond_ces")*p->get<double>("rej_energy_ces"));
    conf.set_rejectionEnergy_cos(p->get<double>("pond_cos")*p->get<double>("rej_energy_cos"));
    conf.set_coefficient_ces(p->get<double>("rej_energy_ces"));
    conf.set_coefficient_cos(p->get<double>("rej_energy_ces")/lot->_rule._cosMax);//(p->get<double>("rej_energy_cos"));

     /*** sampler ***/
    double vLen = sqrt(p->get<double>("maxsize"))/2;
    K::Vector_2 v(vLen,vLen);
    uniform_birth birth(
            Cuboid_bldg(lot->_box2d.min(),-v,1/p->get<double>("maxratio"),p->get<double>("minheight"))
            ,Cuboid_bldg(lot->_box2d.max(),v,  p->get<double>("maxratio"),p->get<double>("maxheight"))
            );

    Filter filter(lot);
    rejection_birth rejection(birth,filter);


    distribution cs(p->get<double>("poisson"));

    d_sampler ds( cs, rejection );

    sampler samp( ds, acceptance()
                , marked_point_process::make_uniform_birth_death_kernel(birth, p->get<double>("pbirth"), p->get<double>("pdeath") )
                , marked_point_process::make_uniform_modification_kernel(cuboid_edge_translation_transform(),0.4)
                //, marked_point_process::make_uniform_modification_kernel(cuboid_corner_translation_transform(),0.4)
                , marked_point_process::make_uniform_modification_kernel(cuboid_height_scaling_transform(p->get<double>("minheight"),p->get<double>("maxheight")),0.8)
                // , 0.5 * modif2
                );

    schedule sch(p->get<double>("temp"),p->get<double>("deccoef"));
    end_test end(p->get<int>("nbiter"));

    typedef rjmcmc::any_sampler<configuration> any_sampler;
    any_sampler anySampler(samp);

    /*< Build and initialize simple visitor which prints some data on the standard output >*/
    typedef simulated_annealing::any_composite_visitor<configuration,any_sampler> composite_visitor;
    composite_visitor visitors;
    visitors.push_back(simulated_annealing::ostream_visitor());
    //visitor.push_back(simulated_annealing::shp::shp_visitor(argv[0]));
    visitors.push_back(simulated_annealing::tex_visitor("cuboid",lot));
    visitors.init(p->get<int>("nbdump"),p->get<int>("nbsave"));

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    std::vector<double> allEnergy;
    simulated_annealing::optimize(conf,anySampler,sch,end,visitors,allEnergy);


    std::ofstream outEnergy("energy.txt");
    if(!outEnergy.is_open())
        std::cout<<"errororor"<<std::endl;
    else{
        std::vector<double>::iterator it;
        for(it=allEnergy.begin();it!=allEnergy.end();++it)
            outEnergy<<*it<<"\n";
    }

    //visualize BLDGS
    std::vector<Bldg> bldgs;

    std::ifstream infile("cuboid000000000149999.tex");
    if(infile.is_open())
    {
        while(!infile.eof())
        {
            std::vector<Point> points;
            double x,y,h;
            for(int i=0;i<5;++i)
            {
                infile>>x;
                infile>>y;
                points.push_back(Point(x,y,0));
            }
            infile>>h;
            LineString ring(points);
            Polygon footprint(ring);
            bldgs.push_back(Bldg(footprint,h,lot));

        }
        bldgs.pop_back();
    }

    Visualize vis;
    vis.display(bldgs,lot,WHITE,AZURE2);

    delete lot;
    return 0;
}

//for test
OGRPolygon* loadFromShp(const char* shpIn)
{
    OGRRegisterAll();
    OGRDataSource *poDS = OGRSFDriverRegistrar::Open(shpIn,FALSE);
    OGRLayer *poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();
    OGRFeature *poFeature = poLayer->GetNextFeature();

    if(!poFeature)
        exit(2);
    if(poFeature->GetGeometryRef()->getGeometryType()!= 3) //3 POLYGON
        exit(3);

    OGRPolygon* ogrPly = (OGRPolygon*)(poFeature->GetGeometryRef()->clone());
    OGRFeature::DestroyFeature(poFeature);
    OGRDataSource::DestroyDataSource(poDS);
    return ogrPly;
}

Lot* singleLotFromShp(const char* shpIn, const char* shpOut)
{
	OGRPolygon * ogrPly;
	{
        OGRRegisterAll();
        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(shpIn,FALSE);
        OGRLayer *poLayer = poDS->GetLayer(0);
        poLayer->ResetReading();
        OGRFeature *poFeature = poLayer->GetNextFeature();

        if(!poFeature)
            exit(2);
        if(poFeature->GetGeometryRef()->getGeometryType()!= 3) //3 POLYGON
            exit(3);

        ogrPly = (OGRPolygon*)(poFeature->GetGeometryRef()->clone());
        OGRFeature::DestroyFeature(poFeature);
        OGRDataSource::DestroyDataSource(poDS);
    }

    /*** create lot  ***/
    /*** counterclockwise ***/
    /*** set leftbottom as origin ***/

    OGREnvelope envelope;
    ogrPly->getEnvelope(&envelope);
    double x0 = envelope.MinX, y0 = envelope.MinY;


    OGRLinearRing* ogrRing = ogrPly->getExteriorRing();
	for(int i=ogrRing->getNumPoints()-1;i>=0;i--) //to counterclockwise
	{
        double x = ogrRing->getX(i)-x0;
        double y = ogrRing->getY(i)-y0;
        ogrRing->setPoint(i,x,y);
	}

//	SFCGAL::LineString ring;
//	OGRLinearRing * ogrRing = ogrPly->getExteriorRing();
//	for(int i=ogrRing->getNumPoints()-1;i>=0;i--) //to counterclockwise
//	{
//        double x = ogrRing->getX(i)-x0;
//        double y = ogrRing->getY(i)-y0;
//        ring.addPoint(SFCGAL::Point(x,y));
//	}
//
//    SFCGAL::Polygon ply(ring);
//	ogrRing->empty();
//
////	//test
////    LineString ring1 = ply.exteriorRing();
////	std::cout<<"num of points "<<ring1.numPoints()<<std::endl;
////	for(int i=0;i<ring1.numPoints();i++)
////	{
////        std::cout<<std::fixed<<ring1.pointN(i).x()<<" "<<ring1.pointN(i).y()<<std::endl;
////	}
//
//
//	if(ogrPly->getNumInteriorRings())
//	{
//        OGRLinearRing* ogrRingIn;
//        for(int i=0;i<ogrPly->getNumInteriorRings();i++)
//        {
//             SFCGAL::LineString ringIn;
//             ogrRingIn = ogrPly->getInteriorRing(i);
//             for(int j=0;j<ogrRingIn->getNumPoints();j++)
//             {
//                ringIn.addPoint(SFCGAL::Point(ogrRingIn->getX(i),ogrRingIn->getY(i),ogrRingIn->getZ(i)));
//             }
//             ogrRingIn->empty();
//             ply.addRing(ringIn);
//        }
//	}
//
//    ogrPly->empty();

    //export translated lot polygon
    {
        if ( boost::filesystem::exists( shpOut ) )
            remove(shpOut);

        OGRRegisterAll();
        OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
        OGRDataSource *poDS = poDriver->CreateDataSource( shpOut, NULL );
        OGRLayer *poLayer = poDS->CreateLayer( "lot1", NULL, wkbPolygon, NULL );
        OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        poFeature->SetGeometry(ogrPly);
        poLayer->CreateFeature(poFeature);
        OGRFeature::DestroyFeature(poFeature);
        OGRDataSource::DestroyDataSource( poDS );
    }
	Iso_rectangle_2 box(0.0,0.0,envelope.MaxX-x0,envelope.MaxY-y0);
   // std::cout<<std::fixed<<box.max().x()<<" "<<box.max().y()<<std::endl;
	RuleLot rule;
	return new Lot(ogrPly,rule,box);
}
