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
			L = vec3(0.0f,0.0f,0.0f);
			return;
		}

		// compute the differences v_j - v_i
		// and store them
		vector<vec3> diffs;
		int next = first;
		do {
			int j = it.current();
			diffs.push_back( verts[j] - verts[vi] );
			next = it.next();
		}
		while (next != first and next != -1);

		// now we can compute the weights,
		// which in this case are uniform
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


} // -- namespace smoothing_private
} // -- namespace smoothing
} // -- namespace algorithms
