#include <geoproc/smoothing/iterative_private.hpp>

// C includes
#include <omp.h>

// C++ includes
#include <iostream>
using namespace std;

// gpr algorithms includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

namespace algorithms {
namespace smoothing {
namespace smoothing_private {

	void make_uniform_weight
	(int vi, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L)
	{
		iterators::vertex::vertex_vertex_iterator it(m);

		int first = it.init(vi);
		if (first == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return;
		}

		// compute the differences v_j - v_i and store them

		// differences vector
		vector<glm::vec3> diffs;
		int j = it.current();
		do {
			diffs.push_back( verts[j] - verts[vi] );
			j = it.next();
		}
		while (j != first and j != -1);

		if (j == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return;
		}

		// now we can compute the weights, which in
		// this case are uniform so only one value
		float w = 1.0f/diffs.size();

		// multiply the weight to each difference
		// and accumulate to L
		for (size_t i = 0; i < diffs.size(); ++i) {
			L += w*diffs[i];
		}
	}

	inline float cotan(float a) { return std::cos(a)/std::sin(a); }

	void make_cotangent_weight
	(int vi, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L)
	{
		const std::vector<glm::vec3>& mesh_angles = m.get_angles();

		iterators::vertex::vertex_face_iterator it(m);
		const int first = it.init(vi);
		int next1 = first;
		int next2 = it.next();

		if (next1 == -1 or next2 == -1) {
			return;
		}

		// differences vector
		vector<glm::vec3> diffs;
		// sum of cotangents vector
		vector<float> weights;
		// sum of all weights
		float S = 0.0f;

		// loop variables
		glm::vec3 diff;
		float alpha, beta;
		do {
			int i1,j1,k1, i2,j2,k2;
			m.get_vertices_triangle(next1, i1,j1,k1);
			m.get_vertices_triangle(next2, i2,j2,k2);

			// Compute the two angles (alpha and beta).
			// At the same time, compute the difference vector.

			const glm::vec3 angles1 = mesh_angles[next1];
			const glm::vec3 angles2 = mesh_angles[next2];

			if (vi == i1)		{ alpha = angles1.y; diff = verts[k1] - verts[vi]; }
			else if (vi == j1)	{ alpha = angles1.z; diff = verts[i1] - verts[vi]; }
			else if (vi == k1)	{ alpha = angles1.x; diff = verts[j1] - verts[vi]; }
			if (vi == i2)		{ beta = angles2.z; }
			else if (vi == j2)	{ beta = angles2.x; }
			else if (vi == k2)	{ beta = angles2.y; }

			diffs.push_back( diff );

			// compute weight
			float W = cotan(alpha) + cotan(beta);
			weights.push_back(W);
			S += W;

			// go to next 2 faces
			next1 = next2;
			next2 = it.next();
		}
		while (next1 != first and next2 != -1);

		if (next1 == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return;
		}

		// calculate vector
		for (size_t i = 0; i < weights.size(); ++i) {
			L += (weights[i]/S)*diffs[i];
		}
	}

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

		glm::vec3 L;
		switch (w) {
			case smooth_weight::uniform:
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = glm::vec3(0.0f,0.0f,0.0f);
					make_uniform_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;

			case smooth_weight::cotangent:
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = glm::vec3(0.0f,0.0f,0.0f);
					make_cotangent_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;

			default:
				cerr << "Warning (in algorithms::smoothing::laplacian)" << endl;
				cerr << "    Value for smoothing weight not recognised" << endl;
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

		glm::vec3 L;
		switch (w) {
			case smooth_weight::uniform:

				#pragma omp parallel for num_threads(nt) private(L)
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = glm::vec3(0.0f,0.0f,0.0f);
					make_uniform_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;

			case smooth_weight::cotangent:

				#pragma omp parallel for num_threads(nt) private(L)
				for (int i = 0; i < m.n_vertices(); ++i) {
					L = glm::vec3(0.0f,0.0f,0.0f);
					make_cotangent_weight(i, m, from, L);
					// apply formula
					to[i] = from[i] + l*L;
				}
				break;

			default:
				cerr << "Warning (in algorithms::smoothing::laplacian)" << endl;
				cerr << "    Value for smoothing weight not recognised" << endl;
		}

	}

} // -- namespace smoothing_private
} // -- namespace smoothing
} // -- namespace algorithms
