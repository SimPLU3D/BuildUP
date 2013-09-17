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
    /*T: Object eg Rectangle*/
    template<typename T, typename UnaryCES, typename UnaryCOS, typename UnaryBorder
    ,typename BinaryOverlap, typename BinaryDistance,typename Accelerator=trivial_accelerator, typename OutEdgeList = boost::listS, typename VertexList = boost::listS  >
    class graph_configuration
    {
    public:
	typedef graph_configuration<T,UnaryCES, UnaryCOS, UnaryBorder, BinaryOverlap, BinaryDistance, Accelerator, OutEdgeList, VertexList> self;
	typedef internal::modification<self>	modification;
	typedef T	value_type;
    private:
	class edge {
	public:
            edge() : m_energy(0),m_energy_overlap(0),m_energy_align(0) {}
            inline double energy() const { return m_energy; }
            inline double energy_overlap() const { return m_energy_overlap;}
            inline double energy_align() const { return m_energy_align;}
            inline void energy(double overlap,double align) { m_energy = overlap+align; m_energy_overlap=overlap; m_energy_align = align;}

	private:
            double m_energy;
            double m_energy_overlap;
            double m_energy_align;
	};

	class node {
	public:
            node(const value_type& obj, double ces,double cos
            ,double MaxCES, double MaxCOS,double rejEnergy_ces,double rejEnergy_cos,double coef_ces,double coef_cos
            ,double energy_border)
            : m_value(obj), m_ces(ces),m_cos(cos),m_energy_border(energy_border)
            {
                m_energy_ces = MaxCES >= m_ces? (MaxCES-m_ces)*coef_ces:rejEnergy_ces;
                m_energy_cos = MaxCOS >= m_cos? (MaxCOS-m_cos)*coef_cos:rejEnergy_cos;
                m_energy = m_energy_ces+m_energy_cos+m_energy_border;
            }

            inline const value_type& value() const { return m_value; }
            inline double energy() const { return m_energy; }
            inline double energy_ces() const { return m_energy_ces;}
            inline double energy_cos() const { return m_energy_cos;}
            inline double energy_border() const { return m_energy_border;}
            inline double ces() const {return m_ces;}
            inline double cos() const {return m_cos;}

	private:
            value_type	m_value;
            double      m_ces;
            double      m_cos;
            double      m_energy_ces;
            double      m_energy_cos;
            double      m_energy_border;
            double		m_energy;
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
	graph_configuration(UnaryCES unary_ces, UnaryCOS unary_cos
	//, double maxces, double maxcos, double rej_ces, double rej_cos
	, UnaryBorder unary_border
	, BinaryOverlap binary_overlap, BinaryDistance binary_distance, Accelerator accelerator=Accelerator())
	: m_ces(0.), m_cos(0.)
	//, m_MaxCES(maxces),m_MaxCOS(maxcos),m_rejEnergy_ces(rej_ces),m_rejEnergy_cos(rej_cos)
	, m_border(0.), m_overlap(0.),m_distance(0.)
	, m_unary_ces(unary_ces), m_unary_cos(unary_cos),m_unary_border(unary_border)
	, m_binary_overlap(binary_overlap), m_binary_distance(binary_distance)
	, m_accelerator(accelerator)
	{}
	~graph_configuration()
	{}

    inline void setMaxCES(double ces){m_MaxCES=ces;}
    inline void setMaxCOS(double cos){m_MaxCOS=cos;}
    inline void set_rejectionEnergy_ces(double e){m_rejEnergy_ces = e;}
    inline void set_rejectionEnergy_cos(double e){m_rejEnergy_cos = e;}
    inline void set_coefficient_ces(double c){m_coef_ces=c;}
    inline void set_coefficient_cos(double c){m_coef_cos=c;}

	// configuration accessors

	inline double ces_energy () const { return (m_MaxCES >= m_ces? (m_MaxCES-m_ces)*m_coef_ces:m_rejEnergy_ces);}
	inline double cos_energy () const { return (m_MaxCOS >= m_cos? (m_MaxCOS-m_cos)*m_coef_cos:m_rejEnergy_cos);}
	inline double border_energy() const { return m_border;}

	inline double overlap_energy() const{ return m_overlap;}
	inline double distance_energy() const{ return m_distance;}

	inline double unary_energy() const { return ces_energy ()+cos_energy () + border_energy();}
	inline double binary_energy() const { return overlap_energy()+distance_energy();}
	inline double energy       () const { return unary_energy()+binary_energy();}

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

    inline double ces(const_iterator v) const{ return m_graph[*v].ces();}
    inline double cos(const_iterator v) const{ return m_graph[*v].cos();}


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
            typedef typename Modification::birth_const_iterator bci;
            typedef typename Modification::death_const_iterator dci;
            bci bbeg = modif.birth_begin();
            bci bend = modif.birth_end();
            dci dbeg = modif.death_begin();
            dci dend = modif.death_end();

            //ces cos value
            double e = 0, o = 0;
            for(bci it=bbeg; it!=bend; ++it)
            {
                e += rjmcmc::apply_visitor(m_unary_ces,*it);
                o += rjmcmc::apply_visitor(m_unary_cos,*it);
            }
            for(dci it=dbeg; it!=dend; ++it)
            {
                e -= ces(*it);
                o -= cos(*it);
            }

            //ces energy
            if(m_MaxCES >= m_ces && m_MaxCES < (m_ces+e))
            {
                delta -= ces_energy();
                delta += m_rejEnergy_ces;
            }
            if(m_MaxCES >= m_ces && m_MaxCES >= (m_ces+e))
            {
                delta -= e*m_coef_ces;
            }
            if(m_MaxCES < m_ces && m_MaxCES >= (m_ces+e))
            {
                delta -= m_rejEnergy_ces;
                delta += (m_MaxCES-m_ces-e)*m_coef_ces;
            }

            //cos energy
            if(m_MaxCOS >= m_cos && m_MaxCOS < (m_cos+o))
            {
                delta -= cos_energy();
                delta += m_rejEnergy_cos;
            }
            if(m_MaxCOS >= m_cos && m_MaxCOS >= (m_cos+o))
            {
                delta -= e*m_coef_cos;
            }
            if(m_MaxCOS < m_cos && m_MaxCOS >= (m_cos+o))
            {
                delta -= m_rejEnergy_cos;
                delta += (m_MaxCOS-m_cos-o)*m_coef_cos;
            }


            //binary birth
            for(bci it=bbeg; it!=bend; ++it)
            {
                const_iterator   it2, end2;
                boost::tie(it2,end2)=m_accelerator(*this,*it);
                for (; it2 != end2; ++it2)
                    if (std::find(dbeg,dend,it2)==dend)
                    {
                        delta += rjmcmc::apply_visitor(m_binary_overlap, *it, value(it2) );
                        delta += rjmcmc::apply_visitor(m_binary_distance, *it, value(it2) );
                    }
                for (bci it2=bbeg; it2 != it; ++it2)
                {
                    delta += rjmcmc::apply_visitor(m_binary_overlap, *it, *it2);
                    delta += rjmcmc::apply_visitor(m_binary_distance, *it, *it2);
                }
            }

            //binary death
            for(dci it=dbeg; it!=dend; ++it) {
                iterator v = *it;
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
	}

	// manipulators
	void insert(const value_type& obj)
	{
            node n(obj, rjmcmc::apply_visitor(m_unary_ces,obj),rjmcmc::apply_visitor(m_unary_cos,obj)
            ,m_MaxCES,m_MaxCOS,m_rejEnergy_ces,m_rejEnergy_cos,m_coef_ces,m_coef_cos
            ,rjmcmc::apply_visitor(m_unary_border,obj));
            m_ces += n.ces();
            m_cos += n.cos();
            m_border += n.energy_border();
            vertex_descriptor d = add_vertex(n, m_graph);
            iterator   it, end;

            for (boost::tie(it,end)=m_accelerator(*this,obj); it != end; ++it) {
                if ( *it == d ) continue;
                double e1 = rjmcmc::apply_visitor(m_binary_overlap, obj, value(it) );
                double e2 = rjmcmc::apply_visitor(m_binary_distance, obj, value(it) );
                if (   e1 == 0 && e2 == 0) continue;
                edge_descriptor_bool new_edge = add_edge(d, *it, m_graph );
                m_graph[ new_edge.first ].energy(e1,e2);
                m_overlap += e1;
                m_distance += e2;
            }
	}

	void remove( iterator v )
	{
            out_edge_iterator it, end;
            for(boost::tie(it,end) = out_edges( *v, m_graph ); it!=end; ++it)
            {
                m_overlap -= m_graph[ *it ].energy_overlap();
                m_distance -= m_graph[ *it ].energy_align();
            }
            m_ces -= m_graph[*v].ces();
            m_cos -= m_graph[*v].cos();
            m_border -= m_graph[*v].energy_border();
            clear_vertex ( *v , m_graph);
            remove_vertex( *v , m_graph);
	}

	inline void clear() { m_graph.clear(); m_ces=m_cos=m_border=m_overlap=m_distance=0; }

	// audit
	double audit_unary_energy() const
	{
            double e = 0., o=0., b = 0.;
            for (const_iterator i=begin(); i != end(); ++i)
            {

                e += rjmcmc::apply_visitor(m_unary_ces, value(i) );
                o += rjmcmc::apply_visitor(m_unary_cos, value(i) );
               // double x = rjmcmc::apply_visitor(m_unary_border, value(i) );
               // b += x;
                b += rjmcmc::apply_visitor(m_unary_border, value(i) );

            }
            double e_ces = m_MaxCES>=e? (m_MaxCES-e)*m_coef_ces:m_rejEnergy_ces;
            double e_cos = m_MaxCOS>=o? (m_MaxCOS-o)*m_coef_cos:m_rejEnergy_cos;

           // std::cout<<"maxces "<< m_MaxCES<< " ces "<<e<<" energy "<<e_ces<<std::endl;
           // std::cout<<"maxcos "<< m_MaxCOS<< " cos "<<o<<" energy "<<e_cos<<std::endl;

            return e_ces+e_cos+b;

	}

	double audit_binary_energy() const
	{
            double e1 = 0., e2=0.;
            const_edge_iterator it, end;
            for(boost::tie(it,end) = edges( m_graph ); it!=end; ++it)
            {
                e1 += rjmcmc::apply_visitor(m_binary_overlap,m_graph[source(*it,m_graph)].value() ,
                                           m_graph[target(*it,m_graph)].value() );
                e2 += rjmcmc::apply_visitor(m_binary_distance,m_graph[source(*it,m_graph)].value() ,
                                           m_graph[target(*it,m_graph)].value() );
             }

             //std::cout<<"intersection energy "<<e1<<std::endl;
             //std::cout<<"distance energy "<<e2<<std::endl;


            return e1+e2;
	}

	unsigned int audit_structure() const
	{
            unsigned int err = 0;
            for (const_iterator i=begin(); i != end(); ++i)
            {
                const_iterator j = i;
                for (++j; j != end(); ++j)
                {
                    bool computed = (0!= rjmcmc::apply_visitor(m_binary_overlap,value(i), value(j)));
                    bool stored = boost::edge(*i, *j, m_graph).second;
                    if (computed != stored)	++err;
                }
            }
            return err;
	}

    private:
	graph_type m_graph;
	UnaryCES m_unary_ces;
	UnaryCOS m_unary_cos;
	UnaryBorder m_unary_border;
	BinaryOverlap m_binary_overlap;
	BinaryDistance m_binary_distance;
	Accelerator	m_accelerator;
	double m_ces; //multiplied by ponderation
	double m_cos; //multiplied by ponderation
	double m_border;
	double m_overlap;
	double m_distance;
	double m_MaxCES; //multiplied by ponderation
	double m_MaxCOS; //multiplied by ponderation
	double m_rejEnergy_ces;//multiplied by ponderation
	double m_rejEnergy_cos;//multiplied by ponderation
	double m_coef_ces;
	double m_coef_cos;
    };


    template<typename T, typename U1, typename U2, typename B1,typename B2, typename A>
    std::ostream& operator<<(std::ostream& o, const graph_configuration<T,U1,U2,B1,B2,A>& c) {
	o << "energy     : " << c.unary_energy() + c.binary_energy();
	o << " = " << c.unary_energy() << " + " << c.binary_energy() << " (Data+Prior)\n";
	o << "Nb objects : " << c.size() << "\n";
	o << "Nb edges   : " << c.size_of_interactions() << std::endl;
	{
            typename graph_configuration<T,U1,U2,B1,B2,A>::const_iterator it = c.begin(), end = c.end();
            for (; it != end; ++it)
                o << *it <<"\t" << c.energy(it)<<"\t" << c[it] << std::endl;
	}

	{
            typename graph_configuration<T,U1,U2,B1,B2,A>::const_edge_iterator it = c.interactions_begin(), end = c.interactions_end();
            for (; it != end; ++it)
                o << *it <<"\t:\t" << c.energy(it) << std::endl;
	}

	return o;
    }

}; // namespace marked_point_process

#endif // GRAPH_CONFIGURATION_HPP
