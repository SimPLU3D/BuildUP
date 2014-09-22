#ifndef MPP_KERNEL_PLU_HPP
#define MPP_KERNEL_PLU_HPP

#include "rjmcmc/random.hpp"
#include "rjmcmc/kernel/kernel.hpp"
#include "rjmcmc/kernel/transform.hpp"
#include <boost/random/variate_generator.hpp>
#include "buildup/plu/Control.hpp"
#include "buildup/plu/Variate.hpp"

namespace marked_point_process
{

// single object type configuration for now...
template<typename T>
class uniform_view
{
    typedef boost::variate_generator<rjmcmc::mt19937_generator&, boost::uniform_smallint<> > die_type;
public:
    enum { dimension =  coordinates_iterator<T>::dimension };
    typedef typename coordinates_iterator<T>::type iterator;

    template<typename Configuration, typename Modification, typename OutputIterator>
    inline double operator()(Configuration& c, Modification& modif, OutputIterator out) const
    {
        //unsigned int n = c.size<T>();
        unsigned int n = c.size();
        if(!n) return 0.;
        //typename Configuration::iterator<T> it = c.begin<T>();
        typename Configuration::iterator it = c.begin();
        die_type die(rjmcmc::random(), boost::uniform_smallint<>(0,n-1));
        std::advance(it, die());

        /*
        // noop
        const T *t = rjmcmc::variant_get<T>(&c[it]);
        if(!t) return 0;
        */
        modif.insert_death(it);
        const T& t = c[it];
//            iterator coord_it  = coordinates_begin(t.rotate(die()));
        iterator coord_it  = coordinates_begin(t);
        for(unsigned int i=0; i<dimension; ++i) *out++ = *coord_it++;
        // return 1./c.size<T>();
        return 1./c.size();
    }
    template<typename Configuration, typename Modification, typename InputIterator>
    inline double inverse_pdf(Configuration& c, Modification& modif, InputIterator it) const
    {
        object_from_coordinates<T> creator;
        modif.insert_birth(creator(it));
        //            return 1./(c.size<T>()+modif.delta_size<T>());
        return 1./(c.size()+modif.delta_size());
    }
};



template<typename T,typename Variate>
class uniform_birth
{
public:
    enum { dimension = coordinates_iterator<T>::dimension };
    typedef T value_type;
    typedef Variate variate_type;
    typedef typename coordinates_iterator<T>::type iterator;
    typedef typename rjmcmc::identity_transform<dimension,double> transform_type;

    uniform_birth(Variate& variate):m_variate(variate){}

    const variate_type& variate() const{return m_variate;}
    const transform_type transform() const{return transform_type();}

    typedef double result_type;

    result_type operator()(T& t) const
    {
        double val[dimension];
        double phi = m_variate(val);
        object_from_coordinates<T> creator;
        t = creator(val);
        return phi;
    }

    struct pdf_visitor
    {
        typedef typename uniform_birth<T,Variate>::result_type result_type;
        const uniform_birth& m_uniform_birth;
        inline result_type operator()(const T &t) const{return m_uniform_birth.pdf(t);}
        pdf_visitor(const uniform_birth& g) : m_uniform_birth(g) {}
    };
    inline pdf_visitor pdf() const{return pdf_visitor(*this);}

    inline result_type pdf(const T &t) const
    {
        iterator val(coordinates_begin(t));
        return m_variate.pdf(val);
    }
private:
    variate_type m_variate;
};

template<typename Birth, typename Predicate>
class rejection_birth
{
public:
    enum { dimension = Birth::dimension };
    typedef typename Birth::value_type value_type;
    typedef typename Birth::result_type result_type;


    rejection_birth(const Birth& b, const Predicate& p, int iter = 100000)
        : m_birth(b), m_predicate(p)
    {
            value_type t;
            int count = 0;
            for(int i=0; i<iter; ++i)
            {
                m_birth(t);
                if(m_predicate(t)) ++count;

            }
            m_renormalization = result_type(iter)/result_type(count);
    }

    struct pdf_visitor
    {
        typedef typename rejection_birth<Birth,Predicate>::result_type result_type;
        const rejection_birth& m_rejection_birth;
        inline result_type operator()(const value_type &t) const
        {
            return m_rejection_birth.pdf(t);
        }
        pdf_visitor(const rejection_birth& g) : m_rejection_birth(g) {}
    };
    inline pdf_visitor pdf() const
    {
        return pdf_visitor(*this);
    }

    inline result_type pdf(const value_type &t) const
    {
        return (m_predicate(t)) ? m_birth.pdf(t)*m_renormalization : 0;
    }

public:
    Birth m_birth;
    Predicate m_predicate;
    // renormalization is equal to 1/integral of pdf of samples that are not rejected
    result_type m_renormalization;
};


//birth death kernel
template <typename birth_type>
        struct uniform_birth_death_kernel
{
    typedef typename birth_type::value_type     value_type;
    typedef typename birth_type::transform_type transform_type;
    typedef rjmcmc::null_view                   view0_type;
    typedef uniform_view<value_type>            view1_type;
    typedef typename birth_type::variate_type   variate0_type;
    typedef rjmcmc::variate<0>                variate1_type;
    typedef rjmcmc::kernel<view0_type,view1_type,variate0_type,variate1_type,transform_type> type;
};

template <typename birth_type>
        typename uniform_birth_death_kernel<birth_type>::type
        make_uniform_birth_death_kernel(const birth_type& b, double p=0.5, double q = 0.5)
{
    typedef uniform_birth_death_kernel<birth_type> res;
    typename res::view0_type view0;
    typename res::view1_type view1;
    typename res::variate1_type variate1;
    return typename res::type(view0,view1,b.variate(),variate1,b.transform(), p, q);
}


//modification kernel

template <typename Transform, typename T = typename Transform::value_type>
struct uniform_modification_kernel
{
    enum { N = Transform::dimension-coordinates_iterator<T>::dimension };
    typedef uniform_view<T>    view_type;
    typedef rjmcmc::variate<N> variate_type;
    typedef rjmcmc::kernel<view_type,view_type,variate_type,variate_type,Transform> type;
};

template <typename Transform>
typename uniform_modification_kernel<Transform>::type
make_uniform_modification_kernel(const Transform& t, double p)
{
    typedef uniform_modification_kernel<Transform> res;
    typename res::view_type view;
    typename res::variate_type variate;
    return typename res::type(view,view,variate,variate,t, p*0.5, p*0.5);
}



}; // namespace rjmcmc

#endif // MPP_KERNEL_HPP
