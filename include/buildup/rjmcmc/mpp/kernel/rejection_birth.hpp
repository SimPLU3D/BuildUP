#ifndef REJECTION_BIRTH_HPP_INCLUDED
#define REJECTION_BIRTH_HPP_INCLUDED

#include <rjmcmc/util/random.hpp>

namespace marked_point_process
{

template<typename Birth, typename Predicate>
class rejection_birth
{
public:
    enum { dimension = Birth::dimension };
    typedef typename Birth::value_type value_type; // object: eg Rectangle
    typedef typename Birth::result_type result_type; // pdf value


    rejection_birth(const Birth& b, const Predicate& p, int iter = 100000)//, result_type renorm)
        : m_birth(b), m_predicate(p)
    {
        value_type t;
        int count = 0;
        for(int i=0; i<iter; ++i)
        {
            typedef rjmcmc::mt19937_generator Engine;
            Engine& e = rjmcmc::random();
            m_birth(e,t);
            if(m_predicate(t)) ++count;

        }
        //m_renormalization = result_type(iter)/result_type(count);
        m_renormalization = result_type(count)/result_type(iter);
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


}; // namespace marked_point_process

#endif // REJECTION_BIRTH_HPP_INCLUDED
