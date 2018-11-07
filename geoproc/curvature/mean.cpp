#include <geoproc/curvature/curvature.hpp>

// C includes
#include <assert.h>
#include <omp.h>

// C++ includes
#include <limits>
#include <cmath>

// glm includes
#include <glm/glm.hpp>

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

namespace algorithms {
namespace curvature {

	inline float cotan(float a) { return std::cos(a)/std::sin(a); }

	inline float Kh_at_vertex(const TriangleMesh& m, int vi) {
		// mesh info
		const std::vector<float>& mesh_areas = m.get_areas();
		const std::vector<glm::vec3>& mesh_angles = m.get_angles();

		iterators::vertex::vertex_face_iterator it(m);
		const int first = it.init(vi);
		int next1 = first;
		int next2 = it.next();

		if (next1 == -1 or next2 == -1) {
			return 0.0f;
		}

		// Voronoi area around 'i'
		float vor_area = 0.0f;
		// curvature vector
		glm::vec3 curv_vec(0.0f,0.0f,0.0f);

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

			if (vi == i1)		{
				alpha = angles1.y;
				diff = m.get_vertex(k1) - m.get_vertex(vi);
			}
			else if (vi == j1)	{
				alpha = angles1.z;
				diff = m.get_vertex(i1) - m.get_vertex(vi);
			}
			else if (vi == k1)	{
				alpha = angles1.x;
				diff = m.get_vertex(j1) - m.get_vertex(vi);
			}
			if (vi == i2)		{ beta = angles2.z; }
			else if (vi == j2)	{ beta = angles2.x; }
			else if (vi == k2)	{ beta = angles2.y; }

			// compute weight
			float W = cotan(alpha) + cotan(beta);
			// accumulate curvature vector
			curv_vec += W*diff;

			// -- Accumulate area --
			// when the triangle has an angle that is larger
			// than 90 degrees (pi/2) then the area contributes
			// only by half.
			float area = mesh_areas[next1];
			vor_area += area;

			// go to next 2 faces
			next1 = next2;
			next2 = it.next();
		}
		while (next1 != first and next2 != -1);

		if (next1 == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return 0.0;
		}

		// finish computation of voronoi area
		vor_area /= 3.0f;
		// finish computation of curvature vector
		curv_vec *= (1.0f/(2.0f*vor_area));

		return (1.0f/2.0f)*glm::length(curv_vec);
	}

	void mean
	(const TriangleMesh& mesh, std::vector<float>& Kh, float *m, float *M) {
		const int N = mesh.n_vertices();
		Kh.resize(N);

		if (m != nullptr) {
			*m = std::numeric_limits<float>::max();
		}
		if (M != nullptr) {
			*M = -std::numeric_limits<float>::max();
		}

		for (int i = 0; i < N; ++i) {
			Kh[i] = Kh_at_vertex(mesh, i);
			if (m != nullptr) {
				*m = std::min(*m, Kh[i]);
			}
			if (M != nullptr) {
				*M = std::max(*M, Kh[i]);
			}
		}
	}

	void mean(const TriangleMesh& mesh, std::vector<float>& Kh, size_t nt) {
		if (nt == 1) {
			mean(mesh, Kh);
			return;
		}

		const int N = mesh.n_vertices();
		Kh.resize(N);

		#pragma omp parallel for num_threads(nt)
		for (int i = 0; i < N; ++i) {
			Kh[i] = Kh_at_vertex(mesh, i);
		}
	}

	void mean
	(const TriangleMesh& mesh, std::vector<float>& Kh, size_t nt, float *m, float *M)
	{
		if (nt == 1) {
			mean(mesh, Kh, m, M);
			return;
		}

		const int N = mesh.n_vertices();
		Kh.resize(N);

		float mm, MM;

		#pragma omp parallel for num_threads(nt) reduction(min:mm) reduction(max:MM)
		for (int i = 0; i < N; ++i) {
			Kh[i] = Kh_at_vertex(mesh, i);
			mm = std::min(mm, Kh[i]);
			MM = std::max(MM, Kh[i]);
		}

		*m = mm;
		*M = MM;
	}

} // -- namespace curavture
} // -- namespace algorithms
