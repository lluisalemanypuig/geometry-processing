#include <geoproc/curvature/curvature.hpp>

// C includes
#include <assert.h>
#include <omp.h>

// C++ includes
#include <limits>
#include <cmath>
using namespace std;

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

inline double cotand(double a) { return std::cos(a)/std::sin(a); }

inline double Kh_at_vertex(const geoproc::TriangleMesh& m, int vi) {
	// mesh info
	const vector<double>& mesh_areas = m.get_areas();
	const vector<vec3d>& mesh_angles = m.get_angles();

	geoproc::iterators::vertex::vertex_face_iterator it(m);
	const int first = it.init(vi);
	int next1 = first;
	int next2 = it.next();

	// Voronoi area around 'i'
	double vor_area = 0.0;
	// curvature vector
	vec3d curv_vec(0.0,0.0,0.0);

	// loop variables
	vec3d diff;
	double alpha = 0.0;
	double beta = 0.0;
	do {
		int i1,j1,k1, i2,j2,k2;
		m.get_vertices_triangle(next1, i1,j1,k1);
		m.get_vertices_triangle(next2, i2,j2,k2);

		// Compute the two angles (alpha and beta).
		// At the same time, compute the difference vector.

		const vec3d& angles1 = mesh_angles[next1];
		const vec3d& angles2 = mesh_angles[next2];

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
		double W = cotand(alpha) + cotand(beta);
		// accumulate curvature vector
		curv_vec += W*diff;

		// -- Accumulate area --
		// when the triangle has an angle that is larger
		// than 90 degrees (pi/2) then the area contributes
		// only by half.
		double area = mesh_areas[next1];
		vor_area += area;

		// go to next 2 faces
		next1 = next2;
		next2 = it.next();
	}
	while (next1 != first);

	// finish computation of voronoi area
	vor_area /= 3.0;
	// finish computation of curvature vector
	curv_vec *= 0.5*vor_area;

	return 0.5*length(curv_vec);
}

namespace geoproc {
namespace curvature {

void mean
(const TriangleMesh& mesh, std::vector<double>& Kh, double *m, double *M) {
	const int N = mesh.n_vertices();
	Kh.resize(N);

	if (m != nullptr) {
		*m = numeric_limits<double>::max();
	}
	if (M != nullptr) {
		*M = -numeric_limits<double>::max();
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

void mean(const TriangleMesh& mesh, std::vector<double>& Kh, size_t nt) {
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
(const TriangleMesh& mesh, std::vector<double>& Kh, size_t nt, double *m, double *M)
{
	if (nt == 1) {
		mean(mesh, Kh, m, M);
		return;
	}

	const int N = mesh.n_vertices();
	Kh.resize(N);

	double mm = std::numeric_limits<double>::max();
	double MM = -mm;

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
} // -- namespace geoproc
