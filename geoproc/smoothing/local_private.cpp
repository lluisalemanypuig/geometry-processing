#include <geoproc/smoothing/local_private.hpp>

// C includes
#include <assert.h>
#include <omp.h>

// C++ includes
#include <vector>

// glm includes
using namespace glm;

// gpr algorithms includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

namespace geoproc {
namespace smoothing {
namespace local_private {

	/* UNIFORM */

	void make_uniform_weights
	(int vi, const TriangleMesh& m, float *pv_ws)
	{
		assert(pv_ws != nullptr);

		iterators::vertex::vertex_vertex_iterator it(m);

		int first = it.init(vi);

		// neighbours of vi
		std::vector<int> neighs;
		int j = it.current();
		do {
			neighs.push_back(j);
			j = it.next();
		}
		while (j != first);

		// set contents of pv_ws to null
		for (int j = 0; j < m.n_vertices(); ++j) {
			pv_ws[j] = 0;
		}
		for (int j : neighs) {
			pv_ws[j] = 1.0f/neighs.size();
		}
	}

	void make_uniform_weight
	(int vi, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L)
	{
		assert(verts != nullptr);

		iterators::vertex::vertex_vertex_iterator it(m);

		int first = it.init(vi);

		// differences vector
		std::vector<vec3> diffs;
		int j = it.current();
		do {
			diffs.push_back( verts[j] - verts[vi] );
			j = it.next();
		}
		while (j != first);

		// now we can compute the weights, which in
		// this case are uniform so only one value
		float w = 1.0f/diffs.size();

		// multiply the weight to each difference
		// and accumulate to L
		for (size_t i = 0; i < diffs.size(); ++i) {
			L += w*diffs[i];
		}
	}

	/* COTANGENT */

	static inline
	float cotan(float a) { return std::cos(a)/std::sin(a); }

	void make_cotangent_weights
	(int vi, const TriangleMesh& m, float *pv_ws)
	{
		assert(pv_ws != nullptr);

		const glm::vec3 *verts = &m.get_vertices()[0];

		iterators::vertex::vertex_face_iterator it(m);
		const int first = it.init(vi);
		int next1 = first;
		int next2 = it.next();

		// loop over the one-ring neighbourhood of
		// vertex vi. When a neighbour is found assign
		// its corresponding weight.

		// loop variables
		vec3 u, v;
		float alpha, beta;
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

			// compute the two angles (alpha and beta).

			// make sure that the orientations are correct.
			// k1 and j2 are the same vertex, and a
			// neighbour of vi.
			assert(k1 == j2);

			// compute the two angles: alpha and beta
			u = normalize( verts[i1] - verts[j1] );
			v = normalize( verts[k1] - verts[j1] );
			alpha = acos( dot(u,v) );
			u = normalize( verts[i2] - verts[k2] );
			v = normalize( verts[j2] - verts[k2] );
			beta = acos( dot(u,v) );

			// compute and assign weight to neighbour
			pv_ws[k1] = cotan(alpha) + cotan(beta);

			// go to next 2 faces
			next1 = next2;
			next2 = it.next();
		}
		while (next1 != first);

	}

	void make_cotangent_weight
	(int vi, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L)
	{
		assert(verts != nullptr);

		iterators::vertex::vertex_face_iterator it(m);
		const int first = it.init(vi);
		int next1 = first;
		int next2 = it.next();

		// differences vector
		std::vector<vec3> diffs;
		// sum of cotangents vector
		std::vector<float> weights;
		// sum of all weights
		float S = 0.0f;

		// loop variables
		vec3 u, v;
		float alpha, beta;
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

			// make sure that the orientations are correct
			assert(k1 == j2);

			// Compute the two angles (alpha and beta).
			u = normalize( verts[i1] - verts[j1] );
			v = normalize( verts[k1] - verts[j1] );
			alpha = acos( dot(u,v) );
			u = normalize( verts[i2] - verts[k2] );
			v = normalize( verts[j2] - verts[k2] );
			beta = acos( dot(u,v) );

			// compute weight
			float W = cotan(alpha) + cotan(beta);
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
		const smooth_weight& w, float l,
		const TriangleMesh& m,
		const glm::vec3 *from, glm::vec3 *to
	)
	{
		assert(from != nullptr);
		assert(to != nullptr);

		// compute the new coordinates of the vertices

		vec3 L;
		switch (w) {
			case smooth_weight::uniform:
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = vec3(0.0f,0.0f,0.0f);
					make_uniform_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;

			case smooth_weight::cotangent:
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = vec3(0.0f,0.0f,0.0f);
					make_cotangent_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;
		}

	}

	void apply_local
	(
		const smooth_weight& w, float l,
		const TriangleMesh& m,
		size_t nt,
		const glm::vec3 *from, glm::vec3 *to
	)
	{
		assert(from != nullptr);
		assert(to != nullptr);

		// compute the new coordinates of the vertices

		vec3 L;
		switch (w) {
			case smooth_weight::uniform:

				#pragma omp parallel for num_threads(nt) private(L)
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = vec3(0.0f,0.0f,0.0f);
					make_uniform_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;

			case smooth_weight::cotangent:

				#pragma omp parallel for num_threads(nt) private(L)
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = vec3(0.0f,0.0f,0.0f);
					make_cotangent_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;
		}

	}

	bool apply_once_per_it
	(
		const smooth_weight& w, float l,
		size_t nit,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
	)
	{
		assert(old_verts != nullptr);
		assert(new_verts != nullptr);

		// if the number of iterations is odd, the for
		// loop below will apply one extra iteration
		if (nit%2 == 1) {
			--nit;
		}

		// for every two iterations, apply two steps, in total nit steps
		size_t it;
		for (it = 0; it < nit; it += 2) {
			local_private::apply_local(w, l, m, old_verts, new_verts);
			local_private::apply_local(w, l, m, new_verts, old_verts);
		}

		// Do one more iteration if necessary.

		if (it == nit) {
			local_private::apply_local(w, l, m, old_verts, new_verts);
			return true;
		}

		return false;
	}

	bool apply_once_per_it
	(
		const smooth_weight& w, float l,
		size_t nit, size_t n_threads,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
	)
	{
		assert(old_verts != nullptr);
		assert(new_verts != nullptr);

		// if the number of iterations is odd, the for
		// loop below will apply one extra iteration
		if (nit%2 == 1) {
			--nit;
		}

		// for every two iterations, apply two steps, in total nit steps
		size_t it;
		for (it = 0; it < nit; it += 2) {
			local_private::apply_local(w, l, m, n_threads, old_verts, new_verts);
			local_private::apply_local(w, l, m, n_threads, new_verts, old_verts);
		}

		// Do one more iteration if necessary.

		if (it == nit) {
			local_private::apply_local(w, l, m, n_threads, old_verts, new_verts);
			return true;
		}

		return false;
	}

	void apply_twice_per_it
	(
		const smooth_weight& w,
		float l1, float l2,
		size_t nit,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
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
		const smooth_weight& w,
		float l1, float l2,
		size_t nit, size_t n_threads,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
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
