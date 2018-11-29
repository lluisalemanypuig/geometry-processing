#include <geoproc/smoothing/global.hpp>

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
	typedef Eigen::MatrixXf Matrixf;
	typedef Eigen::VectorXf Vectorf;
	typedef Eigen::SparseMatrix<float> SparseMatrixf;

	inline void smooth_laplacian
	(
		int N, int variable,
		const smooth_weight& w,
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
		float *ws = (float *)malloc(N*sizeof(float));
		// list of triplets for system's matrix
		vector<T> triplet_list;

		int row_it = 0;
		for (int i = 0; i < N; ++i) {
			if (constant[i]) { continue; }

			// compute weights
			if (w == smooth_weight::uniform) {
				local_private::make_uniform_weights(i, m, ws);
			}
			else if (w == smooth_weight::cotangent) {
				local_private::make_cotangent_weights(i, m, ws);
			}

			ws[i] = -1.0f;

			// compute the right handside of the equation
			// (vectors bX,bY,bZ) using this row
			int col_it = 0;

			glm::vec3 sums(0.0f,0.0f,0.0f);
			for (int j = 0; j < N; ++j) {
				if (constant[j]) {
					// if the vertex at the j-th column is constant
					// we need to accumulate the sum
					sums += ws[j]*m.get_vertex(j);
				}
				else {
					// if it is not constant we have a
					// triplet of the system matrix
					if (ws[j] != 0.0f) {
						triplet_list.push_back(T(row_it, col_it, ws[j]));
					}
					++col_it;
				}
			}

			bX(row_it) = -sums.x;
			bY(row_it) = -sums.y;
			bZ(row_it) = -sums.z;
			++row_it;
		}

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

		// this will never happen
		return false;
	}


} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
