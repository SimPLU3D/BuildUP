#ifndef MAX_NUM_END_TEST_HPP_INCLUDED
#define MAX_NUM_END_TEST_HPP_INCLUDED

namespace simulated_annealing
{
    //if the number of object has reached maximum (nMax), and has lasted for a certain iteration (nDuration)
    class  max_num_end_test
    {
    public:
        max_num_end_test(int nMax,int nDuration):m_max(nMax),m_duration(nDuration),m_i(0){}
        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration& c, const Sampler& s, double)
        {
            m_i = (c.getNumObjects()==m_max)? (m_i+1) : 0;
            return m_i>=m_duration;
        }

    private:
        int m_max, m_duration,m_i;
    };
};

#endif // MAX_NUM_END_TEST_HPP_INCLUDED
