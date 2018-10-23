#include <algorithms/curvature/curvature.hpp>

namespace algorithms {
namespace curvature {

	inline
	float cotan(float a) { return std::cos(a)/std::sin(a); }

	inline
	float Kh_curvature_at(const TriangleMesh& m, int i) {
		// Voronoi area around 'i'
		float area = 0.0f;
		// curvature vector
		vec3 curv_vec(0.0f,0.0f,0.0f);

		iterators::vertex::vertex_face_iterator it(m);
		const int first = it.init(i);
		int next1 = first;
		int next2 = it.next();

		float alpha, beta;
		vec3 u,v;
		do {
			// it is guaranteed that
			//     i1 = i
			//     i2 = i
			// also, faces are sorted in counterclockwise order
			// therefore:
			//      i1 -> j1 -> k1 -> i1
			// (i1) i2 -> j2 -> k2 -> i2 (i1)
			int i1,j1,k1, i2,j2,k2;
			m.get_vertices_face(next1, i, i1,j1,k1);
			m.get_vertices_face(next2, i, i2,j2,k2);

			// make sure that the orientations are correct
			assert(k1 == j2);

			// compute the two angles: alpha and beta
			u = glm::normalize( m.get_vertex(i1) - m.get_vertex(j1) );
			v = glm::normalize( m.get_vertex(k1) - m.get_vertex(j1) );
			beta = glm::acos( glm::dot(u,v) );
			u = glm::normalize( m.get_vertex(i2) - m.get_vertex(k2) );
			v = glm::normalize( m.get_vertex(j2) - m.get_vertex(k2) );
			alpha = glm::acos( glm::dot(u,v) );

			// compute weight
			float W = cotan(alpha) + cotan(beta);
			// accumulate curvature vector
			curv_vec += W*(m.get_vertex(i1) - m.get_vertex(k1));

			// accumulate area
			area += m.get_triangle_area(i1,j1,k1);

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
		area /= 3.0f;
		// finish computation of curvature vector
		curv_vec *= (1.0f/(2.0f*area));

		return (1.0f/2.0f)*glm::length(curv_vec);
	}

	void mean(const TriangleMesh& mesh, vector<float>& Kh, size_t nt) {
		const int N = mesh.n_vertices();
		Kh.resize(N);

		#pragma omp parallel for num_threads(nt)
		for (int i = 0; i < N; ++i) {
			Kh[i] = Kh_curvature_at(mesh, i);
		}
	}

} // -- namespace curavture
} // -- namespace algorithms
