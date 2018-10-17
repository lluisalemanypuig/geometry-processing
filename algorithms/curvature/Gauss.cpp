#include <algorithms/curvature/curvature.hpp>

namespace algorithms {
namespace curvature {

	float Kg_curvature_at(const TriangleMesh& m, int v) {

		// sum of angles incident to 'v'
		float angles = 0.0f;
		// Voronoi area around 'v'
		float area = 0.0f;

		iterators::vertex::vertex_face_iterator it(m);
		it.init(v);
		int first = it.current();
		int next;

		do {
			// current face
			int f = it.current();

			// index vertices of current face
			int i,j,k;
			m.get_vertices_face(f, v, i,j,k);

			// accumulate area
			area += m.get_triangle_area(i,j,k);

			// compute and accumulate angles
			vec3 ij = m.get_vertex(j) - m.get_vertex(i);
			vec3 ik = m.get_vertex(k) - m.get_vertex(i);
			ij = glm::normalize(ij);
			ik = glm::normalize(ik);
			angles += glm::acos( glm::dot(ij,ik) );
		}
		while ((next = it.next()) != first and next != -1);

		if (next == -1) {
			// the computation of the curvature could not
			// be completed since there is a boundary
			return 0.0;
		}

		// finish computation of voronoi area
		area /= 3.0f;

		return (1.0f/area)*(2.0f*M_PI - angles);
	}

	void Gauss(const TriangleMesh& mesh, vector<float>& Kg, size_t nt) {
		size_t N = mesh.n_vertices();
		Kg.resize(N, 0.0);

		#pragma omp parallel for num_threads(nt)
		for (size_t i = 0; i < N; ++i) {
			Kg[i] = Kg_curvature_at(mesh, i);
		}
	}

} // -- namespace curavture
} // -- namespace algorithms
