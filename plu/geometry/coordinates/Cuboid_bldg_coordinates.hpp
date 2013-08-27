#ifndef CUBOID_BLDG_COORDINATES_HPP
#define CUBOID_BLDG_COORDINATES_HPP

#include "geometry/coordinates/coordinates.hpp"
#include "geometry/Cuboid_bldg.hpp"

template<typename K>
struct cuboid_coordinates_iterator
        : public boost::iterator_facade<cuboid_coordinates_iterator<K>, const typename K::FT, boost::forward_traversal_tag>
{
    enum { dimension = 6 };
    cuboid_coordinates_iterator() : m_i(dimension) {}
    explicit cuboid_coordinates_iterator(const geometry::Cuboid_bldg<K>& r) : m_i(0)
    {
        m_coord[0] = r.rect_2.center().x();
        m_coord[1] = r.rect_2.center().y();
        m_coord[2] = r.rect_2.normal().x();
        m_coord[3] = r.rect_2.normal().y();
        m_coord[4] = r.rect_2.ratio();
        m_coord[5] = r.h;
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
struct coordinates_iterator< geometry::Cuboid_bldg<K> >
{
    typedef cuboid_coordinates_iterator<K> type;
    enum { dimension = type::dimension };
};

template<typename K>
struct object_from_coordinates< geometry::Cuboid_bldg<K> > {
    template<typename Iterator> geometry::Cuboid_bldg<K> operator()(Iterator it) {
        typename K::FT x = *it++;
        typename K::FT y = *it++;
        typename K::FT u = *it++;
        typename K::FT v = *it++;
        typename K::FT r = *it++;
        typename K::FT h = *it;
        typename K::Point_2  p(x,y);
        typename K::Vector_2 n(u,v);
        return geometry::Cuboid_bldg<K>(p,n,r,h);
    }
};

#endif // CUBOID_BLDG_COORDINATES_HPP
