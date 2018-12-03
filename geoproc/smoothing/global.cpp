#include <geoproc/smoothing/global.hpp>

// C includes
#include <omp.h>

// C++ includes
#include <iostream>
#include <vector>
using namespace std;

// glm includes
#include <glm/vec3.hpp>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/smoothing/local_private.hpp>

// Eigen includes
#include <Eigen/Sparse>

namespace geoproc {
namespace smoothing {
namespace global {

	typedef Eigen::Triplet<float> T;
	static inline bool comp_triplet(const T& t1, const T& t2) {
		if (t1.row() == t2.row()) {
			return t1.col() < t2.col();
		}
		return t1.row() < t2.row();
	}

	typedef Eigen::MatrixXf Matrixf;
	typedef Eigen::VectorXf Vectorf;
	typedef Eigen::SparseMatrix<float> SparseMatrixf;

	/* ---------------- */
	/* HELPER FUNCTIONS */

	static inline void compute_weights
	(
		int i, const smooth_weight& w,
		const TriangleMesh& m, float *ws
	)
	{
		if (w == smooth_weight::uniform) {
			local_private::make_uniform_weights(i, m, ws);
		}
		else if (w == smooth_weight::cotangent) {
			local_private::make_cotangent_weights(i, m, ws);
		}
		ws[i] = -1.0f;
	}

	static inline void compute_row
	(
		int i, int N, const smooth_weight& w, const vector<bool>& constant,
		const TriangleMesh& m, vector<T>& triplet_list, float *ws,
		int& row_it, int& col_it, glm::vec3& sums
	)
	{
		if (not constant[i]) {
			compute_weights(i, w, m, ws);
		}
		for (int j = 0; j < N; ++j) {
			if (constant[j]) {
				/* if the vertex at the j-th column is constant
					we need to accumulate the sum */
				sums += ws[j]*m.get_vertex(j);
			}
			else {
				/* if it is not constant we have a
				   triplet of the system matrix */
				if (ws[j] != 0.0f) {
					triplet_list.push_back(T(row_it, col_it, ws[j]));
				}
				++col_it;
			}
		}
	}

	static inline void make_triplets
	(
		int a, int b, int N,
		const vector<bool>& constant,
		const smooth_weight& w,
		const TriangleMesh& m,
		float *ws,
		vector<T>& triplet_list,
		Vectorf& bX, Vectorf& bY, Vectorf& bZ
	)
	{
		int row_it = 0;
		for (int i = a; i < b; ++i) {
			if (constant[i]) { continue; }

			glm::vec3 sums(0.0f,0.0f,0.0f);
			int col_it = 0;
			compute_row
			(i, N, w, constant, m, triplet_list, ws, row_it, col_it, sums);

			// compute the right handside of the equation
			// (vectors bX,bY,bZ) using this row
			bX(row_it) = -sums.x;
			bY(row_it) = -sums.y;
			bZ(row_it) = -sums.z;
			++row_it;
		}
	}

	static inline void smooth_laplacian
	(
		int N, int variable, const smooth_weight& w,
		const vector<bool>& constant, TriangleMesh& m
	)
	{
		/* This code was engineered to avoid high memory
		 * consumption (on a system with 8GB the old code
		 * would take up to 44% of the total memory: ~4GB).
		 */

		// system's matrix
		SparseMatrixf A(variable, variable);
		// independent term vectors
		Vectorf bX(variable), bY(variable), bZ(variable);

		// weights for system's matrix (per row)
		float *ws = static_cast<float *>(malloc(N*sizeof(float)));
		// list of triplets for system's matrix
		vector<T> triplet_list;
		// build triplets for system's matrix
		make_triplets
		(0, N, N, constant, w, m, ws, triplet_list, bX, bY, bZ);
		// free memory
		free(ws);

		A.setFromTriplets(triplet_list.begin(), triplet_list.end());
		// free more memory
		triplet_list.clear();
		// improve memory consumption
		A.makeCompressed();

		SparseMatrixf At = A.transpose();

		Eigen::SimplicialCholesky<SparseMatrixf> solver(At*A);
		Vectorf solX = solver.solve(At*bX);
		Vectorf solY = solver.solve(At*bY);
		Vectorf solZ = solver.solve(At*bZ);

		vector<glm::vec3> coords = m.get_vertices();
		int fixed_it = 0;
		for (int i = 0; i < N; ++i) {
			if (not constant[i]) {
				coords[i].x = solX(fixed_it);
				coords[i].y = solY(fixed_it);
				coords[i].z = solZ(fixed_it);
				++fixed_it;
			}
		}
		m.set_vertices(coords);
	}

	bool smooth
	(
		const smooth_operator& op, const smooth_weight& w,
		const vector<bool>& constant, TriangleMesh& m
	)
	{
		if (op == smooth_operator::BiLaplacian) {
			cerr << "Error: global smoothing not implemented for operator"
				 << endl << "    Bi-Laplacian" << endl;
			return false;
		}

		if (op == smooth_operator::TaubinLM) {
			cerr << "Error: invalid value of smoothing operator 'TaubinLM'"
				 << endl << "    for a global smoothing operation" << endl;
			return false;
		}

		const int N = m.n_vertices();
		// amount of constant vertices
		int n_constant = 0;
		for (int i = 0; i < N; ++i) {
			n_constant += constant[i];
		}
		// amount of variable vertices
		const int variable = N - n_constant;

		if (op == smooth_operator::Laplacian) {
			smooth_laplacian(N, variable, w, constant, m);
			return true;
		}

		// this will never happen
		return false;
	}

	/* ------------------ */
	/* PARALLEL SMOOTHING */

	static inline void smooth_laplacian_par
	(
		int N, int variable, const smooth_weight& w,
		const vector<bool>& constant, int nt,
		TriangleMesh& m
	)
	{
		(void)nt;
		smooth_laplacian(N, variable, w, constant, m);
	}

	bool smooth
	(
		const smooth_operator& op, const smooth_weight& w,
		const vector<bool>& constant, size_t n_threads, TriangleMesh& m
	)
	{
		if (n_threads == 1) {
			return smooth(op, w, constant, m);
		}

		if (op == smooth_operator::BiLaplacian) {
			cerr << "Error: global smoothing not implemented for operator"
				 << endl << "    Bi-Laplacian" << endl;
			return false;
		}

		if (op == smooth_operator::TaubinLM) {
			cerr << "Error: invalid value of smoothing operator 'TaubinLM'"
				 << endl << "    for a global smoothing operation" << endl;
			return false;
		}

		const int N = m.n_vertices();
		// amount of constant vertices
		int n_constant = 0;
		#pragma omp parallel for num_threads(n_threads) reduction(+:n_constant)
		for (int i = 0; i < N; ++i) {
			n_constant += constant[i];
		}
		// amount of variable vertices
		const int variable = N - n_constant;

		if (op == smooth_operator::Laplacian) {
			smooth_laplacian_par(N, variable, w, constant, n_threads, m);
			return true;
		}

		// this will never happen
		return false;
	}


} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
