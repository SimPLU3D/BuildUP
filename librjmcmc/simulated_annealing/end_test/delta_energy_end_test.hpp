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

#ifndef DELTA_ENERGY_END_TEST
#define DELTA_ENERGY_END_TEST

namespace simulated_annealing
{
    /**
     * \ingroup GroupEndTest
     *
     * This class is a model of the EndTest concept and stops
     * the simulated annealing process if the energy of the configuration
     * has not changed during the last `n` iterations.
     *
     */
    class delta_energy_end_test {
    public:
        delta_energy_end_test(unsigned int n) : m_i(0), m_n(n) {}
        template<typename Configuration, typename Sampler>
        inline bool operator()(const Configuration&, const Sampler& s, double) {
            m_i = (s.delta()==0)? (m_i+1) : 0;
            return m_i<m_n;
        }
        void stop () { m_n=0; }
    private:
        unsigned int m_i, m_n;
    };

};

#endif // DELTA_ENERGY_END_TEST
