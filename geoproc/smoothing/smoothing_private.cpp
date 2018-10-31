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

		// compute the differences v_j - v_i
		// and store them
		vector<glm::vec3> diffs;
		int j = it.current();
		do {
			diffs.push_back( verts[j] - verts[vi] );
			j = it.next();
		}
		while (j != first and j != -1);

		// now we can compute the weights, which in
		// this case are uniform so only one value
		float w = 1.0f/diffs.size();

		// multiply the weight to each difference
		// and accumulate to L
		for (size_t i = 0; i < diffs.size(); ++i) {
			L += w*diffs[i];
		}
	}

	void make_cotangent_weight
	(int i, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L)
	{

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
