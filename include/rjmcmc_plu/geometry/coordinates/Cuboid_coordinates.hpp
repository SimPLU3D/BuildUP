#ifndef CUBOID_BLDG_COORDINATES_HPP
#define CUBOID_BLDG_COORDINATES_HPP

#include "geometry/coordinates/coordinates.hpp"
#include "rjmcmc_plu/geometry/Cuboid.hpp"
#include <boost/iterator/iterator_facade.hpp>

template<typename K>
struct cuboid_coordinates_iterator
        : public boost::iterator_facade<cuboid_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    enum { dimension = 6 };
    cuboid_coordinates_iterator() : m_i(dimension) {}
    explicit cuboid_coordinates_iterator(const geometry::Cuboid<K>& r) : m_i(0)
    {
        m_coord[0] = r.bottom().center().x();
        m_coord[1] = r.bottom().center().y();
        m_coord[2] = r.rho();
        m_coord[3] = r.theta();
        m_coord[4] = r.ratio();
        m_coord[5] = r.h();
    }
    typedef typename K::FT FT;
private:
    friend class boost::iterator_core_access;
    void increment() { ++m_i; }
    const FT& dereference() const {
        //assert(m_i<dimension);
        return m_coord[m_i];
    }
    FT m_coord[dimension];
    unsigned int m_i;
};

template<typename K>
struct coordinates_iterator< geometry::Cuboid<K> >
{
    typedef cuboid_coordinates_iterator<K> type;
    enum { dimension = type::dimension };
};

template<typename K>
struct object_from_coordinates< geometry::Cuboid<K> > {
    template<typename Iterator> geometry::Cuboid<K> operator()(Iterator it) {
        typename K::FT x = *it++;
        typename K::FT y = *it++;
        typename K::FT rho = *it++;
        typename K::FT theta = *it++;
        typename K::FT r = *it++;
        typename K::FT h = *it;
        typename K::Point_2  p(x,y);
        return geometry::Cuboid<K>(p,rho,theta,r,h);
    }
};

#endif // CUBOID_BLDG_COORDINATES_HPP
