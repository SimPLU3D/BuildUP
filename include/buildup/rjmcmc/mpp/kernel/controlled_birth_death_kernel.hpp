#ifndef CONTROLLED_BIRTH_DEATH_KERNEL_HPP_INCLUDED
#define CONTROLLED_BIRTH_DEATH_KERNEL_HPP_INCLUDED

#include "controlled_view.hpp"
#include <rjmcmc/rjmcmc/kernel/null_view.hpp>
#include <rjmcmc/rjmcmc/kernel/null_variate.hpp>
#include <rjmcmc/rjmcmc/kernel/kernel.hpp>

namespace marked_point_process
{
template <typename birth_type,typename Control>
struct controlled_birth_death_kernel
{
    typedef typename birth_type::value_type         value_type;
    typedef typename birth_type::transform_type     transform_type;
    typedef rjmcmc::null_view                       view0_type;
    typedef controlled_view<value_type,Control,1>   view1_type;
    typedef typename birth_type::variate_type       variate0_type;
    typedef rjmcmc::null_variate                    variate1_type;
    typedef rjmcmc::kernel<view0_type,view1_type,variate0_type,variate1_type,transform_type> type;
};

template <typename birth_type,typename Control>
typename controlled_birth_death_kernel<birth_type,Control>::type
make_controlled_birth_death_kernel(const birth_type& b, const Control& control,double p, double q = 0.5)
{
    typedef controlled_birth_death_kernel<birth_type,Control> res;
    typename res::view0_type view0;
    typename res::view1_type view1(control);
    typename res::variate1_type variate1;
    return typename res::type(view0,view1,b.variate(),variate1,b.transform(), p, q);
}

}; // namespace marked_point_process

#endif // CONTROLLED_BIRTH_DEATH_KERNEL_HPP_INCLUDED
