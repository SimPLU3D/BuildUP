#ifndef BLDG_GENERATOR_HPP
#define BLDG_GENERATOR_HPP

/************ geometry ******************/
#include "rjmcmc_plu/geometry/Cuboid.hpp"
#include "rjmcmc_plu/geometry/coordinates/Cuboid_coordinates.hpp"
#include "rjmcmc_plu/geometry/intersection/Cuboid_intersection.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef geometry::Cuboid<K> Cuboid;
typedef Cuboid object;

/************  energies  ******************/
#include "rjmcmc/energy/energy_operators.hpp"
#include "rjmcmc_plu/mpp/energy/unary_energy_plu.hpp"
#include "rjmcmc_plu/mpp/energy/binary_energy_plu.hpp"
#include "rjmcmc_plu/mpp/energy/global_energy_plu.hpp"

typedef plu_unary_distBorder<> unary_dBorder;
typedef plu_binary_distPair<> binary_dPair;
typedef plu_binary_heightDiff<> binary_hDiff;
typedef plu_global_lcr<> global_lcr;
typedef plu_global_far<> global_far;

/************ configuration ******************/
#include "rjmcmc_plu/mpp/configuration/graph_configuration_plu.hpp"
typedef marked_point_process::graph_configuration_plu<
        object
        ,multiplies_energy<constant_energy<>,unary_dBorder>
        ,multiplies_energy<constant_energy<>,binary_dPair>
        ,multiplies_energy<constant_energy<>,binary_hDiff>
        ,multiplies_energy<constant_energy<>,global_lcr>
        ,multiplies_energy<constant_energy<>,global_far>
        > configuration;


/***** mpp smapler *****/
#include "rjmcmc_plu/mpp/kernel/kernel_plu.hpp"
#include "Control.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
typedef marked_point_process::rejection_birth<uniform_birth,Control_inPolygon> rejection_birth;

#include "rjmcmc/distribution/poisson_distribution.hpp"
typedef rjmcmc::poisson_distribution distribution;

#include "mpp/direct_sampler.hpp"
typedef marked_point_process::direct_sampler<distribution,rejection_birth> d_sampler;



/***** rjmcmc kernels *****/
//typedef marked_point_process::result_of_make_uniform_birth_death_kernel<object>::type  birth_death_kernel;
typedef Control_Rho_Theta_H<Variate_rho_discrete,Variate_theta,Variate_height > Control;
typedef marked_point_process::controlled_birth_death_kernel<object,Control>::type  controlled_birth_death_kernel;

#include "rjmcmc/kernel/transform.hpp"
#include "rjmcmc_plu/geometry/kernels/cuboid_transform_kernel.hpp"
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_edge_translation_transform  >::type  edge_modification_kernel;
typedef marked_point_process::result_of_make_uniform_modification_kernel<cuboid_height_scaling_transform>::type  height_modification_kernel;


/***** rjmcmc sampler *****/
#include "rjmcmc/sampler/sampler.hpp"
#include "rjmcmc/acceptance/metropolis_acceptance.hpp"
typedef rjmcmc::metropolis_acceptance acceptance;

typedef rjmcmc::sampler<d_sampler,acceptance
        //,birth_death_kernel
        ,controlled_birth_death_kernel
        //,edge_modification_kernel
        //,height_modification_kernel
        > sampler;

/***** simulated_annealing *****/
/*< Choice of the schedule (/include/ and /typedef/) >*/
#include "simulated_annealing/schedule/geometric_schedule.hpp"
typedef simulated_annealing::geometric_schedule<double> schedule;
/*< Choice of the end_test (/include/ and /typedef/) >*/

#include "simulated_annealing/end_test/max_iteration_end_test.hpp"
#include "simulated_annealing/end_test/composite_end_test.hpp"

#include "rjmcmc_plu/simulated_annealing/end_test/energy_end_test_plu.hpp"

typedef simulated_annealing::max_iteration_end_test     end_test_nIter;
typedef simulated_annealing::energy_end_test            end_test_energy;

typedef simulated_annealing::composite_end_test<end_test_nIter,end_test_energy> end_test_composite;

#include "simulated_annealing/simulated_annealing.hpp"
#include "rjmcmc/sampler/any_sampler.hpp"
#include "simulated_annealing/visitor/any_visitor.hpp"

#include "rjmcmc_plu/simulated_annealing/visitor/energy_visitor_plu.hpp"
#include "rjmcmc_plu/simulated_annealing/visitor/bldg_visitor_plu.hpp"

#include "param/parameter.hpp"
#include "bldg_generator_para.hpp"

#include "Lot.hpp"
#include "Building.hpp"
#include "statistics.hpp"
#include <ctime>
#include <chrono>
#include <fstream>

configuration* bldg_generator(parameters< parameter >& p, Lot* lot,std::ofstream& ofs_energy,std::ofstream& ofs_bldg)
{

    double eRej = p.get<double>("erej");
   /*** configuration ***/
    configuration* conf = new configuration(
                         p.get<double>("wdborder")*unary_dBorder(lot,eRej)
                        ,p.get<double>("wdpair")*binary_dPair(lot,eRej)
                        ,p.get<double>("whdiff")*binary_hDiff(lot,eRej)
                        ,p.get<double>("wlcr")*global_lcr(lot,eRej)
                        ,p.get<double>("wfar")*global_far(lot,eRej)
                        );

    double initLcrEnergy = p.get<double>("wlcr")*eRej;
    double initFarEnergy = p.get<double>("wfar")*eRej;
    conf->init_energy(initLcrEnergy,initFarEnergy);

     /*** sampler ***/
    Point_2 p1(lot->xMin(),lot->yMin()); //bottom left point of the bounding box
    Point_2 p2(lot->xMax(),lot->yMax()); //upper right point

    //TODO: estimate maxRatio according to parcel shape
    double maxRatio = 5;

    uniform_birth birth(
         Cuboid(p1,lot->ruleGeom()->rhoMin(),0,    1,      lot->ruleGeom()->hMin())
        ,Cuboid(p2,lot->ruleGeom()->rhoMax(),M_PI,maxRatio,lot->ruleGeom()->hMax())
    );


    Control_inPolygon filter(lot->polygon());
    rejection_birth rejection(birth,filter);
    distribution cs(p.get<double>("poisson"));
    d_sampler ds( cs, rejection );

    Variate_rho_discrete vRho(lot->ruleGeom()->rho());
    Variate_theta vTheta(lot);
    Variate_height vH(lot);
    Control control(vRho,vTheta,vH);

    sampler samp( ds, acceptance()
                //, marked_point_process::make_uniform_birth_death_kernel(birth, p.get<double>("pbirth"), p.get<double>("pdeath") )
                , marked_point_process::make_controlled_birth_death_kernel(birth, control, p.get<double>("pbirth"), p.get<double>("pdeath") )
                //, marked_point_process::make_uniform_modification_kernel(cuboid_edge_translation_transform(),0.8)
                //, marked_point_process::make_uniform_modification_kernel(cuboid_height_scaling_transform(lot->_rule._hMin,lot->_rule._hMax,lot->_rule._hFloor),0.6)
                // , 0.5 * modif2
                );

    schedule sch(p.get<double>("temp"),p.get<double>("deccoef"));
    end_test_nIter end_nIter(p.get<int>("nbiter"));
    end_test_energy end_energy(2400,1600,3500);
    end_test_composite end_composite(end_nIter,end_energy);

    typedef rjmcmc::any_sampler<configuration> any_sampler;
    any_sampler anySampler(samp);

    typedef simulated_annealing::any_composite_visitor<configuration,any_sampler> composite_visitor;
    composite_visitor visitors;


    visitors.push_back(simulated_annealing::energy_visitor(ofs_energy));
    visitors.push_back(simulated_annealing::bldg_visitor(ofs_bldg));
    visitors[0].init(0,p.get<int>("nbsave_energy"));
    visitors[1].init(0,p.get<int>("nbsave_bldg"));

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    simulated_annealing::optimize(*conf,anySampler,sch,end_composite,visitors);

    return conf;

}

template<typename Configuration>
void config2bldgs(const Configuration& config,Lot* lot,std::vector<Bldg>& bldgs)
{
    typename Configuration::const_iterator it = config.begin(), end = config.end();
    for (; it != end; ++it)
    {
        std::vector<Point> points;
        for(int i=0;i<5;i++)
        {
            double x = geometry::to_double(config[it].bottom().point(i).x());
            double y = geometry::to_double(config[it].bottom().point(i).y());
            points.push_back(Point(x,y,0));

        }
        LineString ring(points);
        Polygon footprint(ring);
        bldgs.push_back(Bldg(footprint,config[it].h(),lot));
    }

}

void bldg_generator_info(Lot* lot,std::ofstream& ofs_energy,std::ofstream& ofs_bldg,std::vector<Bldg>& bldgs,std::ofstream& ofs_stat,double& lcr,double& far,double& e,double& time)
{
    parameters< parameter > param;
    initialize_parameters(&param);

    std::clock_t c_start = std::clock();
    configuration* conf = bldg_generator(param,lot,ofs_energy,ofs_bldg);
    std::clock_t c_end = std::clock();
    time = (double)(1000.0*(c_end-c_start)/CLOCKS_PER_SEC);

    std::cout<<" CPU time "<<time<<" ms\n";
    ofs_stat<<" CPU time "<<time<<" ms\n";

    io::statistics(param,*conf,lot,ofs_stat,lcr,far,e);
    config2bldgs(*conf,lot,bldgs);

    delete conf;
}
#endif // BLDG_GENERATOR_HPP
