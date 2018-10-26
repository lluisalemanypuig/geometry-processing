#include <algorithms/smoothing/smoothing_private.hpp>

namespace algorithms {
namespace smoothing {
namespace smoothing_private {

	void make_uniform_weight
	(int vi, const TriangleMesh& m, const vector<vec3>& verts, vec3& L)
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
		vector<vec3> diffs;
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
	(int i, const TriangleMesh& m, const vector<vec3>& verts, vec3& L)
	{

	}

	// applies laplacian operator once
	void apply_local
	(
		const smooth_weight& w, float l,
		const TriangleMesh& m,
		const vector<vec3>& from, vector<vec3>& to
	)
	{
		// compute the new coordinates of the vertices
		for (int i = 0; i < m.n_vertices(); ++i) {

			vec3 L(0.0f,0.0f,0.0f);
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
} // -- namespace smoothing
} // -- namespace algorithms
