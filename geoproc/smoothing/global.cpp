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
		/* This code was engineered to avoid high memory
		 * consumption (on a system with 8GB the old code
		 * would take up to 44% of the total memory: ~4GB).
		 */

		// Index for each vertex that points to its
		// corresponding row in the system's matrix.
		// The index is -1 for those constant vertices.
		vector<int> vertices_idxs(N);
		int row_it = 0;
		for (int i = 0; i < N; ++i) {
			if (constant[i]) {
				vertices_idxs[i] = -1;
			}
			else {
				vertices_idxs[i] = row_it;
				++row_it;
			}
		}

		// system's matrix
		SparseMatrixf A(variable, variable);
		// independent term vectors
		Vectorf bX(variable), bY(variable), bZ(variable);

		// list of triplets for system's matrix
		vector<T> triplet_list;

		// "iterator" over the set of vertices per thread
		int iters[nt];
		for (int i = 0; i < nt; ++i) { iters[i] = 0; }

		int chunk_size = N/nt;

		cout << "N= " << N << endl;

		#pragma omp parallel num_threads(nt)
		{
			// weights for system's matrix (per row and thread)
			float *ws = static_cast<float *>(malloc(N*sizeof(float)));

			int id = omp_get_thread_num();
			int inf = id*chunk_size;
			int sup = std::min(N, (id + 1)*chunk_size);

			int& i = iters[id];
			for (i = inf; i < sup; ++i) {

				// 1. compute weights in 'nt' rows in parallel
				if (not constant[i]) {
					compute_weights(i, w, m, ws);
				}

				#pragma omp barrier

				if (not constant[i]) {
					// 2. compute the right handside of the equation
					// (vectors bX,bY,bZ) using this row
					glm::vec3 sums(0.0f,0.0f,0.0f);
					for (int j = 0; j < N; ++j) {
						if (constant[j]) {
							// if the vertex at the j-th column is constant
							// we need to accumulate the sum
							sums += ws[j]*m.get_vertex(j);
						}
					}
					bX(vertices_idxs[i]) = -sums.x;
					bY(vertices_idxs[i]) = -sums.y;
					bZ(vertices_idxs[i]) = -sums.z;

					// compute triplets (only one thread)
					if (id == 0) {
						for (int ii = 0; ii < nt; ++ii) {
							int thread_row = iters[ii];
							if (constant[thread_row]) { continue; }

							for (int j = 0; j < N; ++j) {
								if (constant[j]) { continue; }
								if (ws[j] != 0.0f) { continue; }

								triplet_list.push_back(
									T(vertices_idxs[thread_row],
									  vertices_idxs[j],
									  ws[j])
								);
							}
						}
					}
				}
				#pragma omp barrier
			}
			free(ws);
		}

		/* -------------------- */
		/* -- remaining rows -- */
		// weights for system's matrix (per row)
		float *ws = static_cast<float *>(malloc(N*sizeof(float)));
		// there are missing rows
		int last_sup = (nt - 1 + 1)*chunk_size;
		// build triplets for system's matrix
		make_triplets
		(last_sup, N, N, constant, w, m, ws, triplet_list, bX, bY, bZ);
		// free memory
		free(ws);
		/* -------------------- */

		std::sort(triplet_list.begin(), triplet_list.end(), comp_triplet);

		cout << "Construct system matrix" << endl;

		A.setFromTriplets(triplet_list.begin(), triplet_list.end());
		// free more memory
		triplet_list.clear();
		// improve memory consumption
		A.makeCompressed();

		SparseMatrixf At = A.transpose();

		Eigen::SimplicialCholesky<SparseMatrixf> solver(At*A);

		Vectorf solX, solY, solZ;
		const int threads_solve = std::min(nt, 3);
		#pragma omp parallel num_threads(threads_solve)
		{
			if (omp_get_thread_num() == 0) {
				solX = solver.solve(At*bX);
			}
			else if (omp_get_thread_num() == 1) {
				solY = solver.solve(At*bY);
			}
			else if (omp_get_thread_num() == 2) {
				solZ = solver.solve(At*bZ);
			}
		}

		vector<glm::vec3> coords = m.get_vertices();

		#pragma omp parallel for num_threads(nt)
		for (int i = 0; i < N; ++i) {
			if (vertices_idxs[i] != -1) {
				coords[i].x = solX(vertices_idxs[i]);
				coords[i].y = solY(vertices_idxs[i]);
				coords[i].z = solZ(vertices_idxs[i]);
			}
		}
		m.set_vertices(coords);
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
		#pragma omp paralel for  num_threads(n_threads) reduction(+:n_constant)
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
