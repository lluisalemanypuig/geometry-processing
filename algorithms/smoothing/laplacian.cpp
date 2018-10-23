#include <algorithms/smoothing/smoothing.hpp>

namespace algorithms {
namespace smoothing {

	inline
	void make_uniform_weight(int vi, const TriangleMesh& m, vec3& L) {
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
			diffs.push_back( m.get_vertex(j) - m.get_vertex(vi) );
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

	inline
	void make_cotangent_weight(int i, const TriangleMesh& m, vec3& L) {

	}

	// applies laplacian operator once
	inline void apply_laplacian_iteration(const smooth_weight& w, float l, TriangleMesh& m) {
		// container for the new vertices' coordinates
		vector<vec3> new_vertices(m.n_vertices());

		// compute the new coordinates of the vertices
		for (int i = 0; i < m.n_vertices(); ++i) {

			vec3 L;
			switch (w) {
				case smooth_weight::uniform:
					make_uniform_weight(i, m, L);
					break;
				case smooth_weight::cotangent:
					make_cotangent_weight(i, m, L);
					break;
				default:
					cerr << "Warning (in algorithms::smoothing::laplacian)" << endl;
					cerr << "    Value for smoothing weight not implemented" << endl;
			}

			// aply formula
			new_vertices[i] = m.get_vertex(i) + l*L;
		}

		// replace the coordinates of the old vertices in m
		for (int i = 0; i < m.n_vertices(); ++i) {
			m.set_vertex(i, new_vertices[i]);
		}
	}

	void laplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			apply_laplacian_iteration(w, l, m);
		}

	}

} // -- namespace smoothing
} // -- namespace algorithms
