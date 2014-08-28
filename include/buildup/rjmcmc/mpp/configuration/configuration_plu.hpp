#ifndef RJMCMC_CONFIGURATION_PLU_HPP
#define RJMCMC_CONFIGURATION_PLU_HPP

#include <vector>

namespace marked_point_process
{
//////////////////////////////////////////////////////////

struct trivial_accelerator
{
    template<typename C, typename T> std::pair<typename C::iterator,typename C::iterator> operator()(const C &c, const T &t) const
    {
        return std::make_pair(c.begin(),c.end());
    }
};

namespace internal
{
//        template<typename C> struct inserter {
//            C& c_;
//            inserter(C& c) : c_(c) {}
//            template<typename T> void operator()(T& t) const { c_.insert(t); }
//        };
//        template<typename C> struct remover {
//            C& c_;
//            remover(C& c) : c_(c) {}
//            template<typename T> void operator()(T& t) const { c_.remove(t); }
//        };
//

template<
typename Configuration,
         typename Birth = std::vector<typename Configuration::value_type>,
         typename Death = std::vector<typename Configuration::const_iterator >
         >
class modification
{
public:
    typedef	Birth birth_type;
    typedef Death death_type;
    const birth_type& birth() const
    {
        return m_birth;
    }
    const death_type& death() const
    {
        return m_death;
    }
    birth_type& birth()
    {
        return m_birth;
    }
    death_type& death()
    {
        return m_death;
    }

    // manipulators
    inline void apply(Configuration &c) const
    {
//                std::for_each(m_death.begin(),m_death.end(),internal::remover <Configuration>(c));
//                std::for_each(m_birth.begin(),m_birth.end(),internal::inserter<Configuration>(c));
        c.apply(*this);
    }

private:
    birth_type	m_birth;
    death_type	m_death;
};

}; // namespace internal

}; // namespace marked_point_process

#endif // RJMCMC_CONFIGURATION_HPP
