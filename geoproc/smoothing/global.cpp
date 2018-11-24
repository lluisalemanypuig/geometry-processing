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
#include <Eigen/SparseCholesky>
#include <Eigen/Sparse>
#include <Eigen/Dense>
using namespace Eigen;

namespace geoproc {
namespace smoothing {
namespace global {

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
		cout << "    Building system matrix" << endl;
		#endif

		/* build system matrix */
		MatrixXf A(variable, variable);
		int row_it = 0;

		for (int i = 0; i < N; ++i) {
			// skip constant vertices
			if (constant[i]) { continue; }

			int col_it = 0;
			for (int j = 0; j < N; ++j) {
				// skip constant vertices
				if (constant[j]) { continue; }

				// assign weight of non-constant vertices
				A(row_it, col_it) = ws[i][j];

				++col_it;
			}

			++row_it;
		}

		#if defined (DEBUG)
		cout << "    Building vectors" << endl;
		#endif

		/* build independent term vector */
		VectorXf bX(variable);
		VectorXf bY(variable);
		VectorXf bZ(variable);
		row_it = 0;
		for (int i = 0; i < N; ++i) {
			// skip rows of constant vertices
			if (constant[i]) { continue; }

			glm::vec3 sums(0.0f,0.0f,0.0f);
			for (int j = 0; j < N; ++j) {
				// if the vertex at the j-th column is constant
				// we need to accumulate the sum
				if (constant[j]) {
					sums += ws[i][j]*m.get_vertex(j);
				}
			}

			bX(row_it) = -sums.x;
			bY(row_it) = -sums.y;
			bZ(row_it) = -sums.z;
			++row_it;
		}

		#if defined (DEBUG)
		cout << "    Transpose of system matrix" << endl;
		#endif

		MatrixXf At = A.transpose();

		#if defined (DEBUG)
		cout << "    LDLT decomposition of system matrix" << endl;
		#endif

		auto ldlt = (At*A).ldlt();

		#if defined (DEBUG)
		cout << "    Solving for:" << endl;
		cout << "        x" << endl;
		#endif

		VectorXf X = ldlt.solve(At*bX);

		#if defined (DEBUG)
		cout << "        y" << endl;
		#endif

		VectorXf Y = ldlt.solve(At*bY);

		#if defined (DEBUG)
		cout << "        z" << endl;
		#endif

		VectorXf Z = ldlt.solve(At*bZ);

		#if defined (DEBUG)
		cout << "    Building vertices" << endl;
		#endif

		vector<glm::vec3> coords = m.get_vertices();
		int fixed_it = 0;
		for (int i = 0; i < N; ++i) {
			if (not constant[i]) {
				coords[i] = glm::vec3( X(fixed_it), Y(fixed_it), Z(fixed_it) );
				++fixed_it;
			}
		}
		m.set_vertices(coords);

		return true;
	}


} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
