/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

#include <geoproc/smoothing/local_private.hpp>

// C includes
#include <assert.h>
#include <omp.h>

// C++ includes
#include <vector>
using namespace std;

// glm includes
using namespace glm;

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

inline double cotand(double a) { return std::cos(a)/std::sin(a); }

namespace geoproc {
namespace smoothing {
namespace local_private {

/* UNIFORM */

void make_uniform_weights
(int vi, const TriangleMesh& m, double *pv_ws)
{
	assert(pv_ws != nullptr);

	iterators::vertex::vertex_vertex_iterator it(m);

	int first = it.init(vi);

	// neighbours of vi
	vector<int> neighs;
	int j = it.current();
	do {
		neighs.push_back(j);
		j = it.next();
	}
	while (j != first);

	// set weights to 0
	for (int j = 0; j < m.n_vertices(); ++j) {
		pv_ws[j] = 0.0;
	}
	for (int j : neighs) {
		pv_ws[j] = 1.0/neighs.size();
	}
}

void make_uniform_weight
(int vi, const TriangleMesh& m, const glm::vec3d *verts, glm::vec3d& L)
{
	assert(verts != nullptr);

	iterators::vertex::vertex_vertex_iterator it(m);

	int first = it.init(vi);

	// differences vector
	vector<vec3d> diffs;
	int j = it.current();
	do {
		diffs.push_back( verts[j] - verts[vi] );
		j = it.next();
	}
	while (j != first);

	// now we can compute the weights, which in
	// this case are uniform so only one value
	double w = 1.0/diffs.size();

	// multiply the weight to each difference
	// and accumulate to L
	for (size_t i = 0; i < diffs.size(); ++i) {
		L += w*diffs[i];
	}
}

/* COTANGENT */

void make_cotangent_weights
(int vi, const TriangleMesh& m, double *pv_ws)
{
	assert(pv_ws != nullptr);

	// mesh info
	const vector<vec3d>& mesh_angles = m.get_angles();

	// set weights to 0
	for (int j = 0; j < m.n_vertices(); ++j) {
		pv_ws[j] = 0.0;
	}

	iterators::vertex::vertex_face_iterator it(m);
	const int first = it.init(vi);
	int next1 = first;
	int next2 = it.next();

	// loop over the one-ring neighbourhood of
	// vertex vi. When a neighbour is found assign
	// its corresponding weight.

	// loop variables
	vector<pair<int, double> > weight_per_neigh;
	double alpha = 0.0;
	double beta = 0.0;
	double W = 0.0;
	double sW = 0.0;
	do {
		int i1,j1,k1, i2,j2,k2;
		m.get_vertices_triangle(next1, vi, i1,j1,k1);
		m.get_vertices_triangle(next2, vi, i2,j2,k2);

		// make sure that the orientations are correct.
		// k1 and j2 are the same vertex, and a neighbour of vi.
		assert(i1 == i2);
		assert(k1 == j2);

		// Compute the two angles (alpha and beta).
		// At the same time, compute the difference vector.

		const vec3d& angles1 = mesh_angles[next1];
		const vec3d& angles2 = mesh_angles[next2];

		if (vi == i1)		{ alpha = angles1.y; }
		else if (vi == j1)	{ alpha = angles1.z; }
		else if (vi == k1)	{ alpha = angles1.x; }
		if (vi == i2)		{ beta = angles2.z; }
		else if (vi == j2)	{ beta = angles2.x; }
		else if (vi == k2)	{ beta = angles2.y; }

		// compute and store weight
		W = cotand(alpha) + cotand(beta);
		weight_per_neigh.push_back( make_pair(k1,W) );
		sW += W;

		// go to next 2 faces
		next1 = next2;
		next2 = it.next();
	}
	while (next1 != first);

	for (const pair<int,double>& nw : weight_per_neigh) {
		pv_ws[nw.first] = nw.second/sW;
	}
}

void make_cotangent_weight
(int vi, const TriangleMesh& m, const glm::vec3d *verts, glm::vec3d& L)
{
	assert(verts != nullptr);

	iterators::vertex::vertex_face_iterator it(m);
	const int first = it.init(vi);
	int next1 = first;
	int next2 = it.next();

	// differences vector
	vector<vec3d> diffs;
	// sum of cotangents vector
	vector<double> weights;
	// sum of all weights
	double S = 0.0;

	// loop variables
	vec3d u, v;
	double alpha, beta;
	do {
		// it is guaranteed that
		//     i1 = i
		//     i2 = i
		// also, faces are sorted in counterclockwise order
		// therefore:
		//      i1 -> j1 -> k1 -> i1
		// (i1) i2 -> j2 -> k2 -> i2 (i1)
		int i1,j1,k1, i2,j2,k2;
		m.get_vertices_triangle(next1, vi, i1,j1,k1);
		m.get_vertices_triangle(next2, vi, i2,j2,k2);

		// make sure that the orientations are correct.
		// k1 and j2 are the same vertex, and a neighbour of vi.
		assert(i1 == i2);
		assert(k1 == j2);

		// Compute the two angles (alpha and beta).
		u = normalize( verts[i1] - verts[j1] );
		v = normalize( verts[k1] - verts[j1] );
		alpha = std::acos( dot(u,v) );

		u = normalize( verts[i2] - verts[k2] );
		v = normalize( verts[j2] - verts[k2] );
		beta = std::acos( dot(u,v) );

		// compute weight
		double W = cotand(alpha) + cotand(beta);
		S += W;

		weights.push_back(W);
		// accumulate curvature vector
		diffs.push_back(verts[i1] - verts[k1]);

		// go to next 2 faces
		next1 = next2;
		next2 = it.next();
	}
	while (next1 != first);

	// calculate vector
	for (size_t i = 0; i < weights.size(); ++i) {
		L += (weights[i]/S)*diffs[i];
	}
}

/* APPLY LOCAL ALGORITHMS */

void apply_local
(
	const weight& w, double l,
	const TriangleMesh& m,
	const glm::vec3d *from, glm::vec3d *to
)
{
	assert(from != nullptr);
	assert(to != nullptr);

	// compute the new coordinates of the vertices

	vec3d L;
	switch (w) {
		case weight::uniform:
			for (int i = 0; i < m.n_vertices(); ++i) {
				L = vec3d(0.0,0.0,0.0);
				make_uniform_weight(i, m, from, L);
				// apply formula
				to[i] = from[i] + l*L;
			}
			break;

		case weight::cotangent:
			for (int i = 0; i < m.n_vertices(); ++i) {
				L = vec3d(0.0,0.0,0.0);
				make_cotangent_weight(i, m, from, L);
				// apply formula
				to[i] = from[i] + l*L;
			}
			break;
	}

}

void apply_local
(
	const weight& w, double l,
	const TriangleMesh& m,
	size_t nt,
	const glm::vec3d *from, glm::vec3d *to
)
{
	assert(from != nullptr);
	assert(to != nullptr);

	// compute the new coordinates of the vertices

	vec3d L;
	switch (w) {
		case weight::uniform:

			#pragma omp parallel for num_threads(nt) private(L)
			for (int i = 0; i < m.n_vertices(); ++i) {
				L = vec3d(0.0,0.0,0.0);
				make_uniform_weight(i, m, from, L);
				// apply formula
				to[i] = from[i] + l*L;
			}
			break;

		case weight::cotangent:

			#pragma omp parallel for num_threads(nt) private(L)
			for (int i = 0; i < m.n_vertices(); ++i) {
				L = vec3d(0.0,0.0,0.0);
				make_cotangent_weight(i, m, from, L);
				// apply formula
				to[i] = from[i] + l*L;
			}
			break;
	}

}

bool apply_once_per_it
(
	const weight& w, double l,
	size_t nit,
	const TriangleMesh& m,
	glm::vec3d *old_verts,
	glm::vec3d *new_verts
)
{
	assert(old_verts != nullptr);
	assert(new_verts != nullptr);

	// if the number of iterations is odd, the for
	// loop below will apply one extra iteration

	// for every two iterations, apply two steps, in total nit steps
	size_t it;
	for (it = 0; it < nit - (nit%2 == 1); it += 2) {
		local_private::apply_local(w, l, m, old_verts, new_verts);
		local_private::apply_local(w, l, m, new_verts, old_verts);
	}

	// do one more iteration if necessary.
	if (nit%2 == 1) {
		local_private::apply_local(w, l, m, old_verts, new_verts);
		return true;
	}

	return false;
}

bool apply_once_per_it
(
	const weight& w, double l,
	size_t nit, size_t n_threads,
	const TriangleMesh& m,
	glm::vec3d *old_verts,
	glm::vec3d *new_verts
)
{
	assert(old_verts != nullptr);
	assert(new_verts != nullptr);

	// if the number of iterations is odd, the for
	// loop below will apply one extra iteration

	// for every two iterations, apply two steps, in total nit steps
	size_t it;
	for (it = 0; it < nit - (nit%2 == 1); it += 2) {
		local_private::apply_local(w, l, m, n_threads, old_verts, new_verts);
		local_private::apply_local(w, l, m, n_threads, new_verts, old_verts);
	}

	// do one more iteration if necessary.
	if (nit%2 == 1) {
		local_private::apply_local(w, l, m, n_threads, old_verts, new_verts);
		return true;
	}

	return false;
}

void apply_twice_per_it
(
	const weight& w,
	double l1, double l2,
	size_t nit,
	const TriangleMesh& m,
	glm::vec3d *old_verts,
	glm::vec3d *new_verts
)
{
	// for each iteration of the algorithm
	for (size_t it = 0; it < nit; ++it) {
		local_private::apply_local(w, l1, m, old_verts, new_verts);
		local_private::apply_local(w, l2, m, new_verts, old_verts);
	}
}

void apply_twice_per_it
(
	const weight& w,
	double l1, double l2,
	size_t nit, size_t n_threads,
	const TriangleMesh& m,
	glm::vec3d *old_verts,
	glm::vec3d *new_verts
)
{
	// for each iteration of the algorithm
	for (size_t it = 0; it < nit; ++it) {
		local_private::apply_local(w, l1, m, n_threads, old_verts, new_verts);
		local_private::apply_local(w, l2, m, n_threads, new_verts, old_verts);
	}
}

} // -- namespace local_private
} // -- namespace smoothing
} // -- namespace geoproc
