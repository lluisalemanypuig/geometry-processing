#include <algorithms/smoothing/iterative_local.hpp>

namespace algorithms {
namespace smoothing {

namespace smoothing_private {
	// applies laplacian operator once
	void apply_local
	(
		const smooth_weight& w, float l,
		const TriangleMesh& m,
		vector<vec3>& from, vector<vec3>& to
	)
	{
		// compute the new coordinates of the vertices
		for (int i = 0; i < m.n_vertices(); ++i) {

			vec3 L;
			switch (w) {
				case smooth_weight::uniform:
					make_uniform_weight(i, m, from, L);
					break;
				case smooth_weight::cotangent:
					make_cotangent_weight(i, m, from, L);
					break;
				default:
					cerr << "Warning (in algorithms::smoothing::laplacian)" << endl;
					cerr << "    Value for smoothing weight not implemented" << endl;
			}

			// apply formula
			to[i] = from[i] + l*L;
		}
	}
} // -- namespace smoothing_private

	void laplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		vector<vec3> old_verts;
		old_verts = m.get_vertices();

		// there is no need to fill 'new_verts'
		// when they are initialised
		vector<vec3> new_verts(old_verts.size());

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			smoothing_private::apply_local(w, l, m, old_verts, new_verts);
			old_verts = new_verts;
		}

		// replace the coordinates of the old vertices in m
		m.set_vertices(new_verts);
	}

} // -- namespace smoothing
} // -- namespace algorithms
