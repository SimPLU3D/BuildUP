#ifndef PLU_BINARY_ENERGY_HPP
#define PLU_BINARY_ENERGY_HPP


template<typename Value = double>
class plu_binary_intersection : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t, const T &u) const
    {
        return std::abs(geometry::to_double(geometry::intersection_area(t,u)));
    }

};

template<typename Value = double>
class plu_binary_distance : public rjmcmc::energy<Value>
{
public:
    typedef Value result_type;

    template<typename T>
    inline result_type operator()(const T &t1,const T &t2) const
    {
        result_type sqd = t1.squared_distance_min(t2);
        return sqd<_dMin*_dMin?_eRej:0;
    }

    plu_binary_distance (Value dMin,result_type rejectionEnergy)
    :_dMin(dMin),_eRej(rejectionEnergy){}

private:
    Value _dMin;
    result_type _eRej;
};


#endif // PLU_BINARY_ENERGY_HPP
