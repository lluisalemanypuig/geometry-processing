#include <geoproc/smoothing/global.hpp>

// C++ includes
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

// glm includes
#include <glm/vec3.hpp>
using namespace glm;

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

	bool smooth
	(
		const smooth_operator& op, const smooth_weight& w,
		const std::vector<bool>& constant, TriangleMesh& m
	)
	{
		if (op == smooth_operator::BiLaplacian) {
			cerr << "Error: global smoothing not implemented for 'BiLaplacian' operator"
				 << endl;
			return false;
		}
		if (op == smooth_operator::TaubinLM) {
			cerr << "Error: invalid value of smoothing operator 'TaubinLM'"
				 << endl;
			return false;
		}

		const int N = m.n_vertices();

		// amount of constant vertices
		int n_constant = 0;

		#if defined (DEBUG)
		cout << "    Computing weights..." << endl;
		#endif

		// weights for system's matrix
		vector<vector<float> > ws(N, vector<float>(N, 0.0f));
		if (w == smooth_weight::uniform) {
			for (int i = 0; i < N; ++i) {
				local_private::make_uniform_weights(i, m, &ws[i][0]);
				ws[i][i] = -1.0f;

				n_constant += constant[i];
			}
		}
		else if (w == smooth_weight::cotangent) {
			for (int i = 0; i < N; ++i) {
				local_private::make_cotangent_weights(i, m, &ws[i][0]);
				ws[i][i] = -1.0f;

				n_constant += constant[i];
			}
		}

		const int variable = N - n_constant;

		#if defined (DEBUG)
		cout << "    Global smoothing info:" << endl;
		cout << "    -> Total vertices: " << N << endl;
		cout << "    -> Constant vertices: " << n_constant << endl;
		cout << "    -> Variable vertices: " << variable << endl;
		cout << endl;
		cout << "    Building independent vector and triplets of matrix..." << endl;
		#endif

		vector<T> triplet_list;

		/* reserve independent term vector */
		Eigen::VectorXf b(3*variable);

		/* fill independent term vector
		   and compute triplets for system matrix */

		int row_it = 0;
		for (int i = 0; i < N; ++i) {
			// skip rows of constant vertices
			if (constant[i]) { continue; }

			int col_it = 0;
			glm::vec3 sums(0.0f,0.0f,0.0f);
			for (int j = 0; j < N; ++j) {
				if (constant[j]) {
					// if the vertex at the j-th column is constant
					// we need to accumulate the sum
					sums += ws[i][j]*m.get_vertex(j);
				}
				else {
					// if it is not constant we have a
					// triplet of the system matrix
					if (ws[i][j] != 0.0f) {
						// save some time by not inserting null entries
						triplet_list.push_back(T(row_it    , col_it    , ws[i][j]));
						triplet_list.push_back(T(row_it + 1, col_it + 1, ws[i][j]));
						triplet_list.push_back(T(row_it + 2, col_it + 2, ws[i][j]));
					}
					col_it += 3;
				}
			}

			b(row_it    ) = -sums.x;
			b(row_it + 1) = -sums.y;
			b(row_it + 2) = -sums.z;
			row_it += 3;
		}

		#if defined (DEBUG)
		cout << "    Building system matrix..." << endl;
		#endif

		/* build system matrix */
		SparseMatrixf A(3*variable, 3*variable);
		A.setFromTriplets(triplet_list.begin(), triplet_list.end());

		#if defined (DEBUG)
		cout << "    Transpose of system matrix..." << endl;
		#endif

		SparseMatrixf At = A.transpose();

		#if defined (DEBUG)
		cout << "    Initialise solver..." << endl;
		#endif

		Eigen::SimplicialCholesky<SparseMatrixf> solver(At*A);

		#if defined (DEBUG)
		cout << "    Solving..." << endl;
		#endif

		Vectorf sol = solver.solve(At*b);

		#if defined (DEBUG)
		cout << "    Building vertices..." << endl;
		#endif

		vector<glm::vec3> coords = m.get_vertices();
		int fixed_it = 0;
		for (int i = 0; i < N; ++i) {
			if (not constant[i]) {
				coords[i].x = sol(fixed_it    );
				coords[i].y = sol(fixed_it + 1);
				coords[i].z = sol(fixed_it + 2);
				fixed_it += 3;
			}
		}
		m.set_vertices(coords);

		#if defined (DEBUG)
		cout << "    Done" << endl;
		#endif

		return true;
	}


} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
