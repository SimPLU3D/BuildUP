#ifndef BLDG_GENERATOR_HPP
#define BLDG_GENERATOR_HPP

/************ geometry ******************/
#include "buildup/rjmcmc/geometry/Cuboid.hpp"
#include "buildup/rjmcmc/geometry/coordinates/Cuboid_coordinates.hpp"
#include "buildup/rjmcmc/geometry/intersection/Cuboid_intersection.hpp"
typedef geometry::Simple_cartesian<double> K;
typedef K::Point_2 Point_2;
typedef geometry::Cuboid<K> Cuboid;
typedef Cuboid object;

/************  energies  ******************/
#include <rjmcmc/rjmcmc/energy/energy_operators.hpp>
#include "buildup/rjmcmc/mpp/energy/unary_energy_plu.hpp"
#include "buildup/rjmcmc/mpp/energy/binary_energy_plu.hpp"
#include "buildup/rjmcmc/mpp/energy/global_energy_plu.hpp"

typedef plu_unary_distBorder<> unary_dBorder;
typedef plu_binary_distPair<> binary_dPair;
typedef plu_binary_heightDiff<> binary_hDiff;
typedef plu_global_lcr<> global_lcr;
typedef plu_global_far<> global_far;

/************ configuration ******************/
#include "buildup/rjmcmc/mpp/configuration/graph_configuration_plu.hpp"
typedef marked_point_process::graph_configuration_plu<
object
,multiplies_energy<constant_energy<>,unary_dBorder>
,multiplies_energy<constant_energy<>,binary_dPair>
,multiplies_energy<constant_energy<>,binary_hDiff>
,multiplies_energy<constant_energy<>,global_lcr>
,multiplies_energy<constant_energy<>,global_far>
> configuration;


/***** mpp smapler *****/
#include <rjmcmc/mpp/kernel/uniform_birth.hpp>
#include "buildup/rjmcmc/mpp/kernel/rejection_birth.hpp"
#include "buildup/plu/Predicate.hpp"
typedef marked_point_process::uniform_birth<object> uniform_birth;
typedef marked_point_process::rejection_birth<uniform_birth,Predicate> rejection_birth;

#include <rjmcmc/rjmcmc/distribution/poisson_distribution.hpp>
typedef rjmcmc::poisson_distribution distribution;

#include <rjmcmc/mpp/direct_sampler.hpp>
typedef marked_point_process::direct_sampler<distribution,rejection_birth> mpp_sampler;


/***** rjmcmc kernels *****/
#include <rjmcmc/mpp/kernel/uniform_birth_death_kernel.hpp>
typedef marked_point_process::uniform_birth_death_kernel<uniform_birth>::type uniform_birth_death_kernel;


/***** rjmcmc sampler *****/
#include <rjmcmc/rjmcmc/sampler/sampler.hpp>
#include <rjmcmc/rjmcmc/acceptance/metropolis_acceptance.hpp>
typedef rjmcmc::metropolis_acceptance acceptance;
typedef rjmcmc::sampler<mpp_sampler,acceptance,uniform_birth_death_kernel> sampler;

/***** simulated_annealing *****/
#include <rjmcmc/simulated_annealing/schedule/geometric_schedule.hpp>
typedef simulated_annealing::geometric_schedule<double> schedule;

#include <rjmcmc/simulated_annealing/end_test/max_iteration_end_test.hpp>
typedef simulated_annealing::max_iteration_end_test     end_test;

#include <rjmcmc/simulated_annealing/simulated_annealing.hpp>
#include <rjmcmc/simulated_annealing/visitor/ostream_visitor.hpp>

#include "buildup/plu/Lot.hpp"
#include "bldg_generator.hpp"
#include "buildup/io/file.hpp"

void bldg_generator(Lot* lot)
{
    /*** parameters ***/
    double temp = 150.;
    double deccoef = 0.999999;
    int nbiter = 2000000;
    int nbdump = 100000;
    int nbsave = 3000000;

    double poisson = 4.;
    double pbirth = 0.6;
    double pdeath = 0.4;

    double eRej = 100.;
    double wlcr = 20.;
    double wfar = 50.;
    double wdborder = 30.;
    double wdpair = 20.;
    double whdiff = 20.;

    /*** configuration ***/
    configuration conf(
        wdborder*unary_dBorder(lot,eRej)
        ,wdpair*binary_dPair(lot,eRej)
        ,whdiff*binary_hDiff(lot,eRej)
        ,wlcr*global_lcr(lot,eRej)
        ,wfar*global_far(lot,eRej));

    conf.init_energy(wlcr*eRej,wfar*eRej);

    /*** sampler ***/
    Point_2 p1(lot->xMin(),lot->yMin()); //bottom left point of the bounding box
    Point_2 p2(lot->xMax(),lot->yMax()); //upper right point
    uniform_birth birth(
        Cuboid(p1,lot->ruleGeom()->rhoMin(),0,   1,lot->ruleGeom()->hMin())
        ,Cuboid(p2,lot->ruleGeom()->rhoMax(),M_PI,5,lot->ruleGeom()->hMax())
    );
    Predicate predicate(lot->polygon()); //if inside parcel polygon
    rejection_birth rejection(birth,predicate); //birth per object
    distribution ds(poisson); //number of object
    mpp_sampler mppSamp( ds, rejection );
    sampler samp( mppSamp, acceptance(),marked_point_process::make_uniform_birth_death_kernel(birth, pbirth, pdeath) );

    /*** simulated annealing ***/
    schedule sch(temp,deccoef);
    end_test end(nbiter);

    typedef rjmcmc::mt19937_generator Engine;
    Engine& e = rjmcmc::random();

    simulated_annealing::ostream_visitor osvisitor;
    osvisitor.init(nbdump,nbsave);

    /*< This is the way to launch the optimization process. Here, the magic happen... >*/
    simulated_annealing::optimize(e,conf,samp,sch,end,osvisitor);

    std::cout<<std::fixed<<"trans" <<-(lot->translatedX())<<" "<<-(lot->translatedY())<<"\n";
    io::save_config2shp(conf,"result.shp",lot->id(),-(lot->translatedX()),-(lot->translatedY()));
}
#endif // BLDG_GENERATOR_HPP
