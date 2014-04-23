/***********************************************************************
This file is part of the librjmcmc project source files.

Copyright : Institut Geographique National (2008-2012)
Contributors : Mathieu Brédif, Olivier Tournaire, Didier Boldo
email : librjmcmc@ign.fr

This software is a generic C++ library for stochastic optimization.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use,
modify and/or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and, more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

***********************************************************************/

#ifndef GRAPH_CONFIGURATION_HPP
#define GRAPH_CONFIGURATION_HPP

#include <boost/graph/adjacency_list.hpp>
#include "configuration.hpp"

namespace marked_point_process {

    template<typename T
    , typename UnaryBorderD
    , typename BinaryDistance
    , typename GlobalCES
    , typename GlobalCOS
    , typename Accelerator=trivial_accelerator, typename OutEdgeList = boost::listS, typename VertexList = boost::listS  >
    class graph_configuration
    {
    public:
	typedef graph_configuration<T,UnaryBorderD, BinaryDistance, GlobalCES,GlobalCOS, Accelerator, OutEdgeList, VertexList> self;
	typedef internal::modification<self>	modification;
	typedef T	value_type;
    private:
	class edge {
	public:
            edge() : m_energy(0) {}
            inline double energy() const { return m_energy; }
            inline void energy(double e) { m_energy = e;    }

	private:
            double m_energy;
	};

	class node {
	public:
            node(const value_type& obj, double e) : m_value(obj), m_energy(e) { }
            inline const value_type& value() const { return m_value; }
            inline double energy() const { return m_energy;}

	private:
            value_type	m_value;
            double      m_energy;
	};
	typedef boost::adjacency_list<OutEdgeList, VertexList, boost::undirectedS, node, edge> graph_type;
	typedef typename graph_type::out_edge_iterator	out_edge_iterator;
	typedef	typename graph_type::vertex_descriptor	vertex_descriptor;
	typedef std::pair< typename graph_type::edge_descriptor , bool > edge_descriptor_bool;

    public:
	typedef	typename graph_type::vertex_iterator	iterator;
	typedef	typename graph_type::vertex_iterator	const_iterator;
	typedef typename graph_type::edge_iterator	edge_iterator;
	typedef typename graph_type::edge_iterator	const_edge_iterator;
    public:

	// configuration constructors/destructors
	graph_configuration(UnaryBorderD class_unary_border_d
	, BinaryDistance class_binary_distance
	, GlobalCES class_global_ces
	, GlobalCOS class_global_cos
	, Accelerator accelerator=Accelerator())
	: m_class_unary_border_d(class_unary_border_d)
	, m_class_binary_distance(class_binary_distance)
	, m_class_global_ces(class_global_ces)
	, m_class_global_cos(class_global_cos)
	, m_accelerator(accelerator)
	, m_dBorder(0.)
	, m_dPair(0.)
	, m_ces(0.)
	, m_cos(0.)
	, m_delta_dBorder(0.)
	, m_delta_dPair(0.)
	, m_delta_ces(0.)
	, m_delta_cos(0.)

	{}
	~graph_configuration()
	{}

    void init_energy(double ces,double cos)
    {
        m_ces = ces;
        m_cos = cos;
    }

	// values
	inline size_t size() const { return num_vertices(m_graph); }
	inline bool empty() const {	return (num_vertices(m_graph)==0); }
	inline iterator begin() { return vertices(m_graph).first; }
	inline iterator end  () { return vertices(m_graph).second; }
	inline const_iterator begin() const { return vertices(m_graph).first; }
	inline const_iterator end  () const { return vertices(m_graph).second; }
	inline const value_type& operator[]( const_iterator v ) const { return m_graph[ *v ].value(); }
	inline const value_type& value( const_iterator v ) const { return m_graph[ *v ].value(); }
	inline double energy( const_iterator v ) const { return m_graph[ *v ].energy(); }

	// interactions
	inline size_t size_of_interactions   () const { return num_edges(m_graph);    }
	inline edge_iterator interactions_begin() { return edges(m_graph).first; }
	inline edge_iterator interactions_end  () { return edges(m_graph).second; }
	inline const_edge_iterator interactions_begin() const { return edges(m_graph).first; }
	inline const_edge_iterator interactions_end  () const { return edges(m_graph).second; }
	inline double energy( edge_iterator e ) const { return m_graph[ *e ].energy(); }

	// evaluators

	template <typename Modification> double delta_energy(const Modification &modif)
	{
            clear_delta();
            typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
            typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
            bci bbeg = modif.birth_begin();
            bci bend = modif.birth_end();
            dci dbeg = modif.death_begin();
            dci dend = modif.death_end();
            for(bci it=bbeg; it!=bend; ++it) {
                m_delta_dBorder += rjmcmc::apply_visitor(m_class_unary_border_d,*it);

                const_iterator   it2, end2;
                boost::tie(it2,end2)=m_accelerator(*this,*it); //vertex iterator [first, end)
                for (; it2 != end2; ++it2)
                    if (std::find(dbeg,dend,it2)==dend) //it2 not dead
                        m_delta_dPair += rjmcmc::apply_visitor(m_class_binary_distance, *it, value(it2) );

                for (bci it2=bbeg; it2 != it; ++it2)
                    m_delta_dPair += rjmcmc::apply_visitor(m_class_binary_distance, *it, *it2);

            }

            for(dci it=dbeg; it!=dend; ++it) {
                iterator v = *it;
                m_delta_dBorder -= energy(v);


//                out_edge_iterator it2, end;
//                for(boost::tie(it2,end) = out_edges( *v, m_graph ); it2!=end; ++it2) {
//                    vertex_descriptor dtarget = target(*it2, m_graph);
//                    bool found = false;
//                    for(dci it3=dbeg; it3!=it && !found; ++it3)
//                        found = (**it3 == dtarget);
//                    if (!found)
//                        m_delta_dPair -= m_graph[ *it2 ].energy();
//                }



                const_iterator   it2, begin2=begin(),end2=end();

                for (it2=begin(); it2 != v; ++it2)
                    if(std::find(dbeg,dend,it2)==dend)
                        m_delta_dPair -= rjmcmc::apply_visitor(m_class_binary_distance, value(v), value(it2) );

                it2 = v;
                ++it2;
                for(;it2!=end2;++it2)
                    m_delta_dPair -= rjmcmc::apply_visitor(m_class_binary_distance, value(v), value(it2) );

            }

            m_delta_ces += rjmcmc::apply_visitor(m_class_global_ces,*this,modif) - m_ces;
            m_delta_cos += rjmcmc::apply_visitor(m_class_global_cos,*this,modif) - m_cos;

            return m_delta_dBorder+m_delta_dPair+m_delta_ces+m_delta_cos;
	}

	template <typename Modification> void apply(const Modification &modif)
	{
            typedef typename Modification::birth_const_iterator bci;
            typedef typename Modification::death_const_iterator dci;

            dci dbeg = modif.death_begin();
            dci dend = modif.death_end();
            for(dci dit=dbeg; dit!=dend; ++dit) //remove(*dit);
            {
                clear_vertex ( **dit , m_graph);
                remove_vertex( **dit , m_graph);
            }
            bci bbeg = modif.birth_begin();
            bci bend = modif.birth_end();
            for(bci bit=bbeg; bit!=bend; ++bit) //insert(*bit);
            {
                node n(*bit, rjmcmc::apply_visitor(m_class_unary_border_d,*bit));
                add_vertex(n, m_graph);
            }
//            m_dBorder = audit_dBorder();
//            m_dPair = audit_dPair();
//            m_ces = audit_ces();
//            m_cos = audit_cos();

            m_dBorder += m_delta_dBorder;
            m_dPair = audit_dPair();
            m_ces += m_delta_ces;
            m_cos += m_delta_cos;

	}

	// manipulators
	void insert(const value_type& obj)
	{
            node n(obj, rjmcmc::apply_visitor(m_class_unary_border_d,obj));
            m_dBorder += n.energy();
            vertex_descriptor d = add_vertex(n, m_graph);

            iterator   it, end;

            for (boost::tie(it,end)=m_accelerator(*this,obj); it != end; ++it) {
                if ( *it == d ) continue;

                double e = rjmcmc::apply_visitor(m_class_binary_distance, obj, value(it));
                //if (   e == 0 ) continue;
                edge_descriptor_bool new_edge = add_edge(d, *it, m_graph );
                m_graph[ new_edge.first ].energy( e);
                m_dPair += e;
            }

	}

	void remove( iterator v )
	{
            out_edge_iterator it, end;
            for(boost::tie(it,end) = out_edges( *v, m_graph ); it!=end; ++it)
                m_dPair-= m_graph[*it].energy();



            m_dBorder -= m_graph[*v].energy();
            clear_vertex ( *v , m_graph);
            remove_vertex( *v , m_graph);
	}

	inline void clear() { m_graph.clear(); m_dBorder=m_dPair=m_ces=m_cos=0.; }

    inline void clear_delta(){m_delta_ces=0.;m_delta_cos=0.;m_delta_dBorder=0.;m_delta_dPair=0.;}

    //configuration accessors
	inline double energy_dBorder() const {return m_dBorder;}
	inline double energy_dPair  () const {return m_dPair;}
	inline double energy_ces    () const {return m_ces;}
	inline double energy_cos    () const {return m_cos;}
	inline double energy        () const {return m_dBorder+m_dPair+m_ces+m_cos;}


    inline double audit_dBorder() const
    {
        double e = 0.;
        for (const_iterator i=begin(); i != end(); ++i)
        {
            e += rjmcmc::apply_visitor(m_class_unary_border_d, value(i) );
            //e += rjmcmc::apply_visitor(m_class_unary_border_h, value(i) );
        }
        return e;
    }

    inline double audit_dPair() const
    {
        double e = 0.;
//        const_edge_iterator it, end;
//        for(boost::tie(it,end) = edges( m_graph ); it!=end; ++it)
//        {
//            e += rjmcmc::apply_visitor(m_class_binary_distance,	m_graph[source(*it,m_graph)].value() ,
//                                       m_graph[target(*it,m_graph)].value() );
//        }

        if(size()>1)
        for (const_iterator i=begin(); i != end(); ++i)
        {
            const_iterator j=i;
            j++;
            for(;j!=end();++j)
                e += rjmcmc::apply_visitor(m_class_binary_distance, value(i),value(j) );
        }


        return e;
    }

    inline double audit_ces() const
    {
        modification modif;
        return rjmcmc::apply_visitor(m_class_global_ces,*this,modif);
    }

    inline double audit_cos() const
    {
        modification modif;
        return rjmcmc::apply_visitor(m_class_global_cos,*this,modif);
    }

    inline double audit_energy() const
    {
        return audit_dBorder() + audit_dPair() +audit_ces()+audit_cos();
    }

//	unsigned int audit_structure() const
//	{
//            unsigned int err = 0;
//            for (const_iterator i=begin(); i != end(); ++i)
//            {
//                const_iterator j = i;
//                for (++j; j != end(); ++j)
//                {
//                    bool computed = (0!= rjmcmc::apply_visitor(m_binary_energy,value(i), value(j)));
//                    bool stored = boost::edge(*i, *j, m_graph).second;
//                    if (computed != stored)	++err;
//                }
//            }
//            return err;
//	}

    private:
	graph_type m_graph;
	UnaryBorderD	m_class_unary_border_d;
	BinaryDistance  m_class_binary_distance;
	GlobalCES       m_class_global_ces;
	GlobalCOS       m_class_global_cos;
	Accelerator	m_accelerator;

	double m_dBorder;
	double m_dPair;
	double m_ces;
	double m_cos;

	double m_delta_dBorder;
	double m_delta_dPair;
	double m_delta_ces;
	double m_delta_cos;

    };


//    template<typename T, typename U, typename B, typename A>
//    std::ostream& operator<<(std::ostream& o, const graph_configuration<T,U,B,A>& c) {
//	o << "energy     : " << c.unary_energy() + c.binary_energy();
//	o << " = " << c.unary_energy() << " + " << c.binary_energy() << " (Data+Prior)\n";
//	o << "Nb objects : " << c.size() << "\n";
//	o << "Nb edges   : " << c.size_of_interactions() << std::endl;
//	{
//            typename graph_configuration<T,U,B,A>::const_iterator it = c.begin(), end = c.end();
//            for (; it != end; ++it)
//                o << *it <<"\t" << c.energy(it)<<"\t" << c[it] << std::endl;
//	}
//
//	{
//            typename graph_configuration<T,U,B,A>::const_edge_iterator it = c.interactions_begin(), end = c.interactions_end();
//            for (; it != end; ++it)
//                o << *it <<"\t:\t" << c.energy(it) << std::endl;
//	}
//
//	return o;
//    }

}; // namespace marked_point_process

#endif // GRAPH_CONFIGURATION_HPP
