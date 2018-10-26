#include <algorithms/smoothing/iterative_local.hpp>

namespace algorithms {
namespace smoothing {

	void laplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		std::vector<glm::vec3> old_verts;
		old_verts = m.get_vertices();
		old_verts.shrink_to_fit();

		// there is no need to fill 'new_verts'
		// when they are initialised
		std::vector<glm::vec3> new_verts(old_verts.size());
		new_verts.shrink_to_fit();

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			smoothing_private::apply_local(w, l, m, old_verts, new_verts);
			if (it < nit - 1) {
				// copy 'new_verts' to 'old_verts'
				// only when necessary
				old_verts.assign(new_verts.begin(), new_verts.end());
			}
		}

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'. Notice that we
		// are using 'move' for moving from 'new_verts' to 'old_verts'.
		// That deletes the contents of 'new_verts'
		m.set_vertices(new_verts);
	}

	void bilaplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		std::vector<glm::vec3> old_verts;
		old_verts = m.get_vertices();
		old_verts.shrink_to_fit();

		// there is no need to fill 'new_verts'
		// when they are initialised
		std::vector<glm::vec3> new_verts(old_verts.size());
		new_verts.shrink_to_fit();

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			smoothing_private::apply_local(w,  l, m, old_verts, new_verts);
			smoothing_private::apply_local(w, -l, m, new_verts, old_verts);
		}

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'. Notice that we
		// are using 'move' for moving from 'new_verts' to 'old_verts'.
		// That deletes the contents of 'new_verts'
		m.set_vertices(old_verts);
	}

	void TaubinLM(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		const float Kpb = 0.1;
		float mu = 1.0f/( Kpb - 1.0f/l);

		std::vector<glm::vec3> old_verts;
		old_verts = m.get_vertices();
		old_verts.shrink_to_fit();

		// there is no need to fill 'new_verts'
		// when they are initialised
		std::vector<glm::vec3> new_verts(old_verts.size());
		new_verts.shrink_to_fit();

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			smoothing_private::apply_local(w,  l, m, old_verts, new_verts);
			smoothing_private::apply_local(w, mu, m, new_verts, old_verts);
		}

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'. Notice that we
		// are using 'move' for moving from 'new_verts' to 'old_verts'.
		// That deletes the contents of 'new_verts'
		m.set_vertices(old_verts);
	}

} // -- namespace smoothing
} // -- namespace algorithms
