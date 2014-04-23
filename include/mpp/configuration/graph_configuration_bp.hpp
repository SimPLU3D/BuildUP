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

    const double initEnergy = 20.0;

    template<typename T
    , typename UnaryBorderD//, typename UnaryBorderH
    , typename BinaryOverlap, typename BinaryDistance
    , typename GlobalCES, typename GlobalCOS
    , typename Accelerator=trivial_accelerator, typename OutEdgeList = boost::listS, typename VertexList = boost::listS  >
    class graph_configuration
    {
    public:
	typedef graph_configuration<T,UnaryBorderD,BinaryOverlap, BinaryDistance,GlobalCES,GlobalCOS, Accelerator, OutEdgeList, VertexList> self;
	typedef internal::modification<self>	modification;
	typedef T	value_type;
    private:
	class edge {
	public:
            edge() : m_energy_distance(0),m_energy_overlap(0){}
            inline double energy() const { return m_energy_overlap+m_energy_distance; }
            inline double energy_overlap() const {return m_energy_overlap;}
            inline double energy_distance() const {return m_energy_distance;}
            inline void energy(double e_d,double e_o) { m_energy_distance=e_d; m_energy_overlap = e_o;   }

	private:
            double m_energy_distance;
            double m_energy_overlap;
	};

	class node {
	public:
            node(const value_type& obj, double e) : m_value(obj), m_energy(e){ }
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
	graph_configuration(UnaryBorderD class_unary_border_d//, UnaryBorderH class_unary_border_h
	, BinaryOverlap class_binary_overlap, BinaryDistance class_binary_distance
	, GlobalCES class_global_ces, GlobalCOS class_global_cos
	, Accelerator accelerator=Accelerator())
	: m_unary_border_d(initEnergy)//, m_unary_border_h(0.)
	, m_binary_overlap(initEnergy), m_binary_distance(initEnergy)
	, m_global_ces(initEnergy), m_global_cos(initEnergy)
	, m_class_unary_border_d(class_unary_border_d)
	//, m_class_unary_border_h(class_unary_border_h)
	, m_class_binary_overlap(class_binary_overlap)
	, m_class_binary_distance(class_binary_distance)
	, m_class_global_ces(class_global_ces)
	, m_class_global_cos(class_global_cos)
	, m_accelerator(accelerator)
	{}
	~graph_configuration()
	{}

    void initWeight(double wUDist,double wOverlap,double wBDist,double wCES,double wCOS)
    {
        m_unary_border_d *= wUDist;
        m_binary_overlap *= wOverlap;
        m_binary_distance *= wBDist;
        m_global_ces *= wCES;
        m_global_cos *= wCOS;
    }
	// configuration accessors
	inline double unary_energy () const { return m_unary_border_d;}
	inline double binary_energy() const { return m_binary_overlap+m_binary_distance;}
	inline double global_energy() const { return m_global_ces+m_global_cos;}
	inline double energy       () const {
            return unary_energy()+binary_energy()+global_energy();
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

	template <typename Modification> double delta_energy(const Modification &modif) const
	{
            double delta = 0;
            typedef typename Modification::birth_const_iterator bci; //iterator of value_type (object)
            typedef typename Modification::death_const_iterator dci; //iterator of vertex_iterator
            bci bbeg = modif.birth_begin();
            bci bend = modif.birth_end();
            dci dbeg = modif.death_begin();
            dci dend = modif.death_end();
            for(bci it=bbeg; it!=bend; ++it) {
                delta += rjmcmc::apply_visitor(m_class_unary_border_d,*it);
                //delta += rjmcmc::apply_visitor(m_class_unary_border_h,*it);
                const_iterator   it2, end2;
                boost::tie(it2,end2)=m_accelerator(*this,*it); //vertex iterator [first, end)
                for (; it2 != end2; ++it2)
                    if (std::find(dbeg,dend,it2)==dend) //it2 not dead
                    {
                        delta += rjmcmc::apply_visitor(m_class_binary_distance, *it, value(it2) );
                        delta += rjmcmc::apply_visitor(m_class_binary_overlap, *it, value(it2) );
                    }

                for (bci it2=bbeg; it2 != it; ++it2)
                {
                    delta += rjmcmc::apply_visitor(m_class_binary_distance, *it, *it2);
                    delta += rjmcmc::apply_visitor(m_class_binary_overlap, *it, *it2);
                }
            }

            for(dci it=dbeg; it!=dend; ++it) {
                iterator v = *it;
                delta -= energy(v);
                out_edge_iterator it2, end;
                for(boost::tie(it2,end) = out_edges( *v, m_graph ); it2!=end; ++it2) {
                    vertex_descriptor dtarget = target(*it2, m_graph);
                    bool found = false;
                    for(dci it3=dbeg; it3!=it && !found; ++it3)
                        found = (**it3 == dtarget);
                    if (!found)
                        delta -= m_graph[ *it2 ].energy();
                }
            }

            delta += rjmcmc::apply_visitor(m_class_global_ces,*this,modif) - m_global_ces;
            delta += rjmcmc::apply_visitor(m_class_global_cos,*this,modif) - m_global_cos;

            return delta;
	}

	template <typename Modification> void apply(const Modification &modif)
	{
            typedef typename Modification::birth_const_iterator bci;
            typedef typename Modification::death_const_iterator dci;
            dci dbeg = modif.death_begin();
            dci dend = modif.death_end();
            for(dci dit=dbeg; dit!=dend; ++dit) remove(*dit);
            bci bbeg = modif.birth_begin();
            bci bend = modif.birth_end();
            for(bci bit=bbeg; bit!=bend; ++bit) insert(*bit);

            m_global_ces = rjmcmc::apply_visitor(m_class_global_ces,*this,modif);
            m_global_cos = rjmcmc::apply_visitor(m_class_global_cos,*this,modif);
	}

	// manipulators
	void insert(const value_type& obj)
	{
            node n(obj, rjmcmc::apply_visitor(m_class_unary_border_d,obj));//,rjmcmc::apply_visitor(m_class_unary_border_h,obj));
            m_unary_border_d += n.energy();
            //m_unary_border_h += n.energy_border_h();

            vertex_descriptor d = add_vertex(n, m_graph);
            iterator   it, end;

            for (boost::tie(it,end)=m_accelerator(*this,obj); it != end; ++it) {
                if ( *it == d ) continue;

                double e1 = rjmcmc::apply_visitor(m_class_binary_distance, obj, value(it));
                double e2 = rjmcmc::apply_visitor(m_class_binary_overlap, obj, value(it));
                if (   e1 == 0 && e2==0) continue;
                edge_descriptor_bool new_edge = add_edge(d, *it, m_graph );
                m_graph[ new_edge.first ].energy( e1,e2 );
                m_binary_distance += e1;
                m_binary_overlap += e2;
            }
	}

	void remove( iterator v )
	{
            out_edge_iterator it, end;
            for(boost::tie(it,end) = out_edges( *v, m_graph ); it!=end; ++it)
            {
                m_binary_overlap -= m_graph[*it].energy_overlap();
                m_binary_distance -= m_graph[*it].energy_distance();
            }
            m_unary_border_d -= m_graph[*v].energy();
            //m_unary_border_h -= m_graph[*v].energy_border_h();
            clear_vertex ( *v , m_graph);
            remove_vertex( *v , m_graph);
	}

	inline void clear() { m_graph.clear(); m_unary_border_d=m_binary_distance=m_binary_overlap=m_global_ces=m_global_cos=0; }


    private:
	graph_type m_graph;
	UnaryBorderD	m_class_unary_border_d;
	//UnaryBorderH    m_class_unary_border_h;
	BinaryOverlap	m_class_binary_overlap;
	BinaryDistance  m_class_binary_distance;
	GlobalCES       m_class_global_ces;
	GlobalCOS       m_class_global_cos;
	Accelerator	m_accelerator;
	double m_unary_border_d;
	//double m_unary_border_h;
	double m_binary_overlap;
	double m_binary_distance;
	double m_global_ces;
	double m_global_cos;
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
