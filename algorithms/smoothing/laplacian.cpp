#include <algorithms/smoothing/iterative_local.hpp>

#include <iostream>

namespace algorithms {
namespace smoothing {

	void laplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		const int N = m.n_vertices();

		// Allocate memory for two arrays of vertices.
		glm::vec3 *old_verts = (glm::vec3 *)malloc(N*sizeof(glm::vec3));
		glm::vec3 *new_verts = (glm::vec3 *)malloc(N*sizeof(glm::vec3));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), &old_verts[0]);

		// if the number of iterations is odd, the for
		// loop below will apply one extra iteration
		if (nit%2 == 1) {
			--nit;
		}

		// for every two iterations, apply two steps.
		size_t it;
		for (it = 0; it < nit; it += 2) {
			smoothing_private::apply_local(w, l, m, old_verts, new_verts);
			smoothing_private::apply_local(w, l, m, new_verts, old_verts);
		}

		// Do one more iteration if necessary. Then,
		// replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'. Notice that we
		// are using 'move' for moving from 'new_verts' to 'old_verts'.
		// That deletes the contents of 'new_verts'

		if (it == nit) {
			// do one last iteration
			smoothing_private::apply_local(w, l, m, old_verts, new_verts);
			m.set_vertices(new_verts, N);
		}
		else {
			m.set_vertices(old_verts, N);
		}

		// free memory
		free(old_verts);
		free(new_verts);
	}

	void bilaplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		const int N = m.n_vertices();

		// Allocate memory for two arrays of vertices.
		glm::vec3 *old_verts = (glm::vec3 *)malloc(N*sizeof(glm::vec3));
		glm::vec3 *new_verts = (glm::vec3 *)malloc(N*sizeof(glm::vec3));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), &old_verts[0]);

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			smoothing_private::apply_local(w,  l, m, old_verts, new_verts);
			smoothing_private::apply_local(w, -l, m, new_verts, old_verts);
		}

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'. Notice that we
		// are using 'move' for moving from 'new_verts' to 'old_verts'.
		// That deletes the contents of 'new_verts'
		m.set_vertices(old_verts, N);

		// free memory
		free(old_verts);
		free(new_verts);
	}

	void TaubinLM(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		const int N = m.n_vertices();
		const float Kpb = 0.1;
		const float mu = 1.0f/(Kpb - 1.0f/l);

		// Allocate memory for two arrays of vertices.
		glm::vec3 *old_verts = (glm::vec3 *)malloc(N*sizeof(glm::vec3));
		glm::vec3 *new_verts = (glm::vec3 *)malloc(N*sizeof(glm::vec3));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), &old_verts[0]);

		// for each iteration of the algorithm
		for (size_t it = 0; it < nit; ++it) {
			smoothing_private::apply_local(w,  l, m, old_verts, new_verts);
			smoothing_private::apply_local(w, mu, m, new_verts, old_verts);
		}

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'. Notice that we
		// are using 'move' for moving from 'new_verts' to 'old_verts'.
		// That deletes the contents of 'new_verts'
		m.set_vertices(old_verts, N);

		// free memory
		free(old_verts);
		free(new_verts);
	}

} // -- namespace smoothing
} // -- namespace algorithms
