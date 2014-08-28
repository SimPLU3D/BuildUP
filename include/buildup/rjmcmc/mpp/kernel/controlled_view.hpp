#ifndef CONTROLLED_VIEW_HPP_INCLUDED
#define CONTROLLED_VIEW_HPP_INCLUDED


#include <boost/random/uniform_smallint.hpp>
#include <rjmcmc/geometry/coordinates/coordinates.hpp>

namespace marked_point_process
{


// single object type configuration for now...
template<typename T, typename Control, unsigned int N=1>
class controlled_view
{
    Control m_control;

public:
    controlled_view(const Control& control):m_control(control) {}

    typedef T object_type;
    enum { dimension =  coordinates_iterator<T>::dimension };

    template<typename Engine, typename Configuration, typename Modification, typename OutputIterator>
    inline double operator()(Engine& e, Configuration const& c, Modification& m, OutputIterator out) const
    {
        m.death().clear();
        typedef typename coordinates_iterator<T>::type iterator;
        unsigned int n = c.size();
        if(n<N) return 0.;
        unsigned int denom=1;
        int d[N];
        for(unsigned int i=0 ; i<N ; ++i,--n)
        {
            boost::uniform_smallint<> die(0,n-1);
            d[i]=die(e);
            for(unsigned int j=0; j<i; ++j) if(d[j]<=d[i]) ++d[i]; // skip already selected indices

            typename Configuration::const_iterator it = c.begin();
            std::advance(it, d[i]);
            m.death().push_back(it);
            const T& t = c.value(it);
            iterator coord_it  = coordinates_begin(t,e);
            for(unsigned int j=0; j<dimension; ++j) *out++ = *coord_it++;
            denom *= n;
        }
        return 1./denom;
    }
    template<typename Configuration, typename Modification, typename InputIterator>
    inline double inverse_pdf(Configuration const& c, Modification& m, InputIterator it) const
    {
        m.birth().clear();
        unsigned int beg   = c.size()-m.death().size()+1;
        unsigned int end   = beg+N;
        unsigned int denom = 1;
        object_from_coordinates<T> creator;
        for(unsigned int n=beg ; n<end ; ++n)
        {
            m_control.apply(it);
            m.birth().push_back(creator(it));
            it    += dimension;
            denom *= n;
        }
        return 1./denom;
    }
};

}; // namespace marked_point_process

#endif // CONTROLLED_VIEW_HPP_INCLUDED
