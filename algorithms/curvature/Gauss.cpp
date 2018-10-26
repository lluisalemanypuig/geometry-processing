#include <algorithms/curvature/curvature.hpp>

namespace algorithms {
namespace curvature {

	/* ---------------------------- */
	/* --------- PARALLEL --------- */

	void Gauss(const TriangleMesh& m, vector<float>& Kg) {
		const int nT = m.n_triangles();
		const int nV = m.n_vertices();

		// Gauss curvature per vertex
		Kg.resize(nV);

		// Angle around each vertex.
		// Define this array to contain:
		// angles[i] = 2*pi - sum_{face j adjacent to i} angle_j
		// where angle_j is the j-th angle incident to vertex i
		// for j = 1 to number of adjacent faces to vertex i.
		vector<float> angles(nV, 2.0f*M_PI);

		int i0,i1,i2;
		float area;
		vec3 u,v;

		// Compute sum of areas of triangles
		for (int t = 0; t < nT; ++t) {
			m.get_vertices_triangle(t, i0,i1,i2);

			// When iterating around a vertex, we will compute
			// the areas of the adjacent triangles. For several
			// vertices, each area will be computed three times.
			// Avoid redundant computation by computing the
			// area of a triangle once and accumulating it to
			// each of its vertices.
			area = m.get_triangle_area(i0,i1,i2);
			Kg[i0] += area;
			Kg[i1] += area;
			Kg[i2] += area;

			// Similarly for the angles.

			const vec3& v0 = m.get_vertex(i0);
			const vec3& v1 = m.get_vertex(i1);
			const vec3& v2 = m.get_vertex(i2);

			// angle <1,0,2>
			u = glm::normalize( v1 - v0 );
			v = glm::normalize( v2 - v0 );
			angles[i0] -= glm::acos( glm::dot(u,v) );

			// angle <0,1,2>
			u = glm::normalize( v0 - v1 );
			v = glm::normalize( v2 - v1 );
			angles[i1] -= glm::acos( glm::dot(u,v) );

			// angle <1,2,0>
			u = glm::normalize( v0 - v2 );
			v = glm::normalize( v1 - v2 );
			angles[i2] -= glm::acos( glm::dot(u,v) );
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

	inline
	float Kg_curvature_at(const TriangleMesh& m, int v) {

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

		vec3 ij,ik;
		int next;
		do {
			// current face
			int f = it.current();

			// index vertices of current face
			int i,j,k;
			m.get_vertices_triangle(f, v, i,j,k);

			// accumulate area
			vor_area += m.get_triangle_area(i,j,k);

			// compute and accumulate angles
			ij = glm::normalize( m.get_vertex(j) - m.get_vertex(i) );
			ik = glm::normalize( m.get_vertex(k) - m.get_vertex(i) );
			angle_sum += glm::acos( glm::dot(ij,ik) );

			next = it.next();
		}
		while (next != first and next != -1);

		if (next == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return 0.0;
		}

		// finish computation of voronoi area
		vor_area /= 3.0f;

		return (1.0f/vor_area)*(2.0f*M_PI - angle_sum);
	}

	void Gauss(const TriangleMesh& m, vector<float>& Kg, size_t nt) {
		if (nt == 1) {
			Gauss(m, Kg);
			return;
		}

		const int N = m.n_vertices();
		Kg.resize(N);

		#pragma omp parallel for num_threads(nt)
		for (int i = 0; i < N; ++i) {
			Kg[i] = Kg_curvature_at(m, i);
		}
	}

} // -- namespace curavture
} // -- namespace algorithms
