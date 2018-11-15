#include <geoproc/smoothing/local.hpp>

// C includes
#include <stdlib.h>
#include <omp.h>

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/smoothing/local_private.hpp>

namespace geoproc {
namespace smoothing {
namespace local {

	void laplacian(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		const int N = m.n_vertices();

		// Allocate memory for two arrays of vertices.
		vec3 *old_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		vec3 *new_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

		bool res = local_private::apply_once_per_it(w, l, nit, m, old_verts, new_verts);
		if (res) {
			m.set_vertices(new_verts, N);
		}
		else {
			m.set_vertices(old_verts, N);
		}

		// free memory
		free(old_verts);
		free(new_verts);
	}

	void laplacian(const smooth_weight& w, float l, size_t nit, size_t nt, TriangleMesh& m) {
		if (nt == 1) {
			laplacian(w,l,nit, m);
			return;
		}

		const int N = m.n_vertices();

		// Allocate memory for two arrays of vertices.
		vec3 *old_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		vec3 *new_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

		bool res = local_private::apply_once_per_it(w, l, nit, nt, m, old_verts, new_verts);
		if (res) {
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
		vec3 *old_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		vec3 *new_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

		local_private::apply_twice_per_it(w, l, -l, nit, m, old_verts, new_verts);

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'.
		m.set_vertices(old_verts, N);

		// free memory
		free(old_verts);
		free(new_verts);
	}

	void bilaplacian(const smooth_weight& w, float l, size_t nit, size_t nt, TriangleMesh& m) {
		if (nt == 1) {
			bilaplacian(w,l,nit, m);
			return;
		}

		const int N = m.n_vertices();

		// Allocate memory for two arrays of vertices.
		vec3 *old_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		vec3 *new_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

		local_private::apply_twice_per_it(w, l, -l, nit,  nt, m, old_verts, new_verts);

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'.
		m.set_vertices(old_verts, N);

		// free memory
		free(old_verts);
		free(new_verts);
	}

	void TaubinLM(const smooth_weight& w, float l, size_t nit, TriangleMesh& m) {
		const int N = m.n_vertices();
		const float Kpb = 0.1f;
		const float mu = 1.0f/(Kpb - 1.0f/l);

		// Allocate memory for two arrays of vertices.
		vec3 *old_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		vec3 *new_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

		local_private::apply_twice_per_it(w, l, mu, nit, m, old_verts, new_verts);

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'.
		m.set_vertices(old_verts, N);

		// free memory
		free(old_verts);
		free(new_verts);
	}

	void TaubinLM(const smooth_weight& w, float l, size_t nit, size_t nt, TriangleMesh& m) {
		if (nt == 1) {
			TaubinLM(w,l,nit, m);
			return;
		}

		const int N = m.n_vertices();
		const float Kpb = 0.1f;
		const float mu = 1.0f/(Kpb - 1.0f/l);

		// Allocate memory for two arrays of vertices.
		vec3 *old_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		vec3 *new_verts = static_cast<vec3 *>(malloc(N*sizeof(vec3)));
		// Fill the first array (there is no need to fill the second).
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

		local_private::apply_twice_per_it(w, l, mu, nit, nt, m, old_verts, new_verts);

		// Replace the coordinates of the old vertices in the
		// mesh for the vertices in 'old_verts'.
		m.set_vertices(old_verts, N);

		// free memory
		free(old_verts);
		free(new_verts);
	}

} // -- namespace local
} // -- namespace smoothing
} // -- namespace geoproc
