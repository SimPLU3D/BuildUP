#ifndef GEOMETRY_CUBOID_BLDG_INTERSECTION_HPP
#define GEOMETRY_CUBOID_BLDG_INTERSECTION_HPP

#include "geometry/intersection/Rectangle_2_intersection.hpp"
namespace geometry {

    /****************************************/
    /* intersection      free function      */
    /****************************************/

    template<class K> typename K::FT intersection_area(const Cuboid_bldg<K>& a, const Cuboid_bldg<K> &b)
    {
        return intersection_area(a.rect_2,b.rect_2);
    }

    /****************************************************/
    /* do_intersect free functions             */
    /* return  intersection_area(a,b)>0;                */
    /****************************************************/

    template <class K> inline bool do_intersect(const Cuboid_bldg<K> &a, const Cuboid_bldg<K> &b)
    {
        return do_intersect(a.rect_2,b.rect_2);
    }

} // namespace geometry

#endif // GEOMETRY_CUBOID_BATI_INTERSECTION_HPP
