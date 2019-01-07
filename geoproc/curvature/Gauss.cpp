#include <geoproc/curvature/curvature.hpp>

// C includes
#include <assert.h>
#include <omp.h>

// C++ includes
#include <iostream>
#include <limits>
#include <cmath>
using namespace std;

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

inline float Kg_at_vertex_par(const geoproc::TriangleMesh& m, int v) {
	// mesh info
	const vector<float>& mesh_areas = m.get_areas();
	const vector<vec3>& mesh_angles = m.get_angles();

	// sum of angles incident to 'v'
	float angle_sum = 0.0f;
	// Voronoi area around 'v'
	float vor_area = 0.0f;

	geoproc::iterators::vertex::vertex_face_iterator it(m);

	int first = it.init(v);

	int f = it.current();
	do {
		// process current face (f)

		// -- Accumulate area --
		// when the triangle has an angle that is larger
		// than 90 degrees (pi/4) then the area contributes
		// only by half.
		float area = mesh_areas[f];
		vor_area += area;

		// index vertices of current face
		int i,j,k;
		m.get_vertices_triangle(f, i,j,k);

		// accumulate angles
		if (v == i)		 { angle_sum += mesh_angles[f].x; }
		else if (v == j) { angle_sum += mesh_angles[f].y; }
		else if (v == k) { angle_sum += mesh_angles[f].z; }

		f = it.next();
	}
	while (f != first);

	// finish computation of voronoi area
	vor_area /= 3.0f;

	return (1.0f/vor_area)*(2.0f*M_PI - angle_sum);
}

namespace geoproc {
namespace curvature {

/* ------------------------------ */
/* --------- SEQUENTIAL --------- */

// Use a different algorithm from the one used for
// the parallel computation of the curvature
void Gauss
(const TriangleMesh& mesh, std::vector<float>& Kg, float *m, float *M)
{
	// mesh info
	const int nT = mesh.n_triangles();
	const int nV = mesh.n_vertices();
	const vector<float>& mesh_areas = mesh.get_areas();
	const vector<vec3>& mesh_angles = mesh.get_angles();

	// Gauss curvature per vertex
	Kg = vector<float>(nV, 0.0f);

	// Angle around each vertex.
	// Define this array to contain:
	// angles[i] = 2*pi - sum_{face j adjacent to i} angle_j
	// where angle_j is the j-th angle incident to vertex i
	// for j = 1 to number of adjacent faces to vertex i.
	vector<float> angles(nV, 2.0f*M_PI);

	int i0,i1,i2;
	float area;

	// Compute sum of areas of triangles
	// and angle around each vertex
	for (int t = 0; t < nT; ++t) {
		mesh.get_vertices_triangle(t, i0,i1,i2);

		// -- Accumulate areas --
		// when the triangle has an angle that is larger
		// than 90 degrees (pi/4) then the area contributes
		// only by half.
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

	// once the angles have been computed, and the
	// areas stored in the curvature vector (to avoid
	// too much memory consumption), calculate the
	// actual value of the curvature:

	if (m != nullptr) {
		*m = numeric_limits<float>::max();
	}
	if (M != nullptr) {
		*M = -numeric_limits<float>::max();
	}

	// compute curvature per vertex
	for (int i = 0; i < nV; ++i) {
		Kg[i] = 3.0f*(angles[i]/Kg[i]);

		if (m != nullptr) {
			*m = std::min(*m, Kg[i]);
		}
		if (M != nullptr) {
			*M = std::max(*M, Kg[i]);
		}
	}
}

/* ---------------------------- */
/* --------- PARALLEL --------- */

void Gauss(const TriangleMesh& m, std::vector<float>& Kg, size_t nt) {
	if (nt == 1) {
		Gauss(m, Kg);
		return;
	}

	const int N = m.n_vertices();
	Kg = vector<float>(N, 0.0f);

	#pragma omp parallel for num_threads(nt)
	for (int i = 0; i < N; ++i) {
		Kg[i] = Kg_at_vertex_par(m, i);
	}
}

void Gauss
(const TriangleMesh& mesh, std::vector<float>& Kg, size_t nt, float *m, float *M)
{
	if (nt == 1) {
		Gauss(mesh, Kg, m, M);
		return;
	}

	const int N = mesh.n_vertices();
	Kg = vector<float>(N, 0.0f);

	float mm = std::numeric_limits<float>::max();
	float MM = -mm;

	#pragma omp parallel for num_threads(nt) reduction(min:mm) reduction(max:MM)
	for (int i = 0; i < N; ++i) {
		Kg[i] = Kg_at_vertex_par(mesh, i);
		mm = std::min(mm, Kg[i]);
		MM = std::max(MM, Kg[i]);
	}

	*m = mm;
	*M = MM;
}

} // -- namespace curavture
} // -- namespace geoproc
