#include <algorithms/curvature/curvature.hpp>

namespace algorithms {
namespace curvature {

	inline
	float Kg_curvature_at(const TriangleMesh& m, int v) {

		// sum of angles incident to 'v'
		float angles = 0.0f;
		// Voronoi area around 'v'
		float area = 0.0f;

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
			area += m.get_triangle_area(i,j,k);

			// compute and accumulate angles
			ij = glm::normalize( m.get_vertex(j) - m.get_vertex(i) );
			ik = glm::normalize( m.get_vertex(k) - m.get_vertex(i) );
			angles += glm::acos( glm::dot(ij,ik) );

			next = it.next();
		}
		while (next != first and next != -1);

		if (next == -1) {
			// the computation of the curvature could not
			// be completed since a boundary was found
			return 0.0;
		}

		// finish computation of voronoi area
		area /= 3.0f;

		return (1.0f/area)*(2.0f*M_PI - angles);
	}

	void Gauss(const TriangleMesh& mesh, vector<float>& Kg) {
		const int N = static_cast<int>(mesh.n_vertices());
		Kg.resize(N);

		for (int i = 0; i < N; ++i) {
			Kg[i] = Kg_curvature_at(mesh, i);
		}
	}

	void Gauss(const TriangleMesh& mesh, vector<float>& Kg, size_t nt) {
		const int N = static_cast<int>(mesh.n_vertices());
		Kg.resize(N);

		#pragma omp parallel for num_threads(nt)
		for (int i = 0; i < N; ++i) {
			Kg[i] = Kg_curvature_at(mesh, i);
		}
	}

} // -- namespace curavture
} // -- namespace algorithms
