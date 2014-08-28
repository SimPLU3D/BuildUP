#ifndef GEOMETRY_CUBOID_INTERSECTION_HPP
#define GEOMETRY_CUBOID_INTERSECTION_HPP

#include <rjmcmc/geometry/intersection/Rectangle_2_intersection.hpp>
namespace geometry
{

/****************************************/
/* intersection      free function      */
/****************************************/

template<class K> typename K::FT intersection_area(const Cuboid<K>& a, const Cuboid<K> &b)
{
    return intersection_area(a.bottom(),b.bottom());
}

/****************************************************/
/* do_intersect free functions             */
/* return  intersection_area(a,b)>0;                */
/****************************************************/

template <class K> inline bool do_intersect(const Cuboid<K> &a, const Cuboid<K> &b)
{
    return do_intersect(a.bottom(),b.bottom());
}

} // namespace geometry

#endif // GEOMETRY_CUBOID_INTERSECTION_HPP
