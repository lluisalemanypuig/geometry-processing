#include <geoproc/curvature/curvature.hpp>

// C includes
#include <assert.h>
#include <omp.h>

// C++ includes
#include <iostream>
#include <cmath>

// glm includes
#include <glm/glm.hpp>

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

namespace algorithms {
namespace curvature {

	/* ------------------------------ */
	/* --------- SEQUENTIAL --------- */

	// Use a different algorithm from the one used for
	// the parallel computation of the curvature
	void Gauss(const TriangleMesh& m, std::vector<float>& Kg) {
		// mesh info
		const int nT = m.n_triangles();
		const int nV = m.n_vertices();
		const std::vector<float>& mesh_areas = m.get_areas();
		const std::vector<glm::vec3>& mesh_angles = m.get_angles();

		// Gauss curvature per vertex
		Kg = std::vector<float>(nV, 0.0f);

		// Angle around each vertex.
		// Define this array to contain:
		// angles[i] = 2*pi - sum_{face j adjacent to i} angle_j
		// where angle_j is the j-th angle incident to vertex i
		// for j = 1 to number of adjacent faces to vertex i.
		std::vector<float> angles(nV, 2.0f*M_PI);

		int i0,i1,i2;
		float area;

		// Compute sum of areas of triangles
		// and angle around each vertex
		for (int t = 0; t < nT; ++t) {
			m.get_vertices_triangle(t, i0,i1,i2);

			// get area of triangle
			area = mesh_areas[t];
			Kg[i0] += area;
			Kg[i1] += area;
			Kg[i2] += area;

			// Similarly for the angles.

			// angle <1,0,2>
			angles[i0] -= mesh_angles[t].x;

			// angle <0,1,2>
			angles[i1] -= mesh_angles[t].y;

			// angle <1,2,0>
			angles[i2] -= mesh_angles[t].z;
		}

		// once the angles have computed, and the area
		// stored in the curvature vector (to avoid
		// too much memory consumption), calculate the
		// actual value of the curvature:

		// compute curvature per vertex
		for (int i = 0; i < nV; ++i) {
			Kg[i] = 3.0f*(angles[i]/Kg[i]);
		}
	}

	/* ---------------------------- */
	/* --------- PARALLEL --------- */

	inline float Kg_at_vertex_par(const TriangleMesh& m, int v) {
		// mesh info
		const std::vector<float>& mesh_areas = m.get_areas();
		const std::vector<glm::vec3>& mesh_angles = m.get_angles();

		// sum of angles incident to 'v'
		float angle_sum = 0.0f;
		// Voronoi area around 'v'
		float vor_area = 0.0f;

		iterators::vertex::vertex_face_iterator it(m);

		int first = it.init(v);
		if (first == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return 0.0;
		}

		int f = it.current();
		do {
			// process current face (f)

			// accumulate area
			vor_area += mesh_areas[f];

			// index vertices of current face
			int i,j,k;
			m.get_vertices_triangle(f, i,j,k);

			// accumulate angles
			if (v == i)		 { angle_sum += mesh_angles[f].x; }
			else if (v == j) { angle_sum += mesh_angles[f].y; }
			else if (v == k) { angle_sum += mesh_angles[f].z; }

			f = it.next();
		}
		while (f != first and f != -1);

		if (f == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return 0.0;
		}

		// finish computation of voronoi area
		vor_area /= 3.0f;

		return (1.0f/vor_area)*(2.0f*M_PI - angle_sum);
	}

	void Gauss(const TriangleMesh& m, std::vector<float>& Kg, size_t nt) {
		if (nt == 1) {
			Gauss(m, Kg);
			return;
		}

		const int N = m.n_vertices();
		Kg = std::vector<float>(N, 0.0f);

		#pragma omp parallel for num_threads(nt)
		for (int i = 0; i < N; ++i) {
			Kg[i] = Kg_at_vertex_par(m, i);
		}
	}

} // -- namespace curavture
} // -- namespace algorithms
