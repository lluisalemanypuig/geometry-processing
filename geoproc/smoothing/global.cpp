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

namespace geoproc {
namespace smoothing {
namespace global {

	typedef Eigen::SparseMatrix<float> SparseMatrixf;

	bool full_smooth
	(const smooth_operator& op, const smooth_weight& w, TriangleMesh& m)
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

		/* build system matrix */
		SparseMatrixf A(3*N, 3*N);
		if (w == smooth_weight::uniform) {
			for (int i = 0; i < N; ++i) {
				vector<float> row(N, 0.0f);
				local_private::make_uniform_weights(i, m, &row[0]);
				for (int j = 0; j < N; ++j) {
					A.insert(3*i    , 3*j    ) = row[j];
					A.insert(3*i + 1, 3*j + 1) = row[j];
					A.insert(3*i + 2, 3*j + 2) = row[j];
				}
				A.coeffRef(3*i    , 3*i    ) = -1.0f;
				A.coeffRef(3*i + 1, 3*i + 1) = -1.0f;
				A.coeffRef(3*i + 2, 3*i + 2) = -1.0f;
			}
		}
		else if (w == smooth_weight::cotangent) {
			for (int i = 0; i < N; ++i) {
				vector<float> row(N, 0.0f);
				local_private::make_cotangent_weights(i, m, &row[0]);
				for (int j = 0; j < N; ++j) {
					A.insert(3*i    , 3*j    ) = row[j];
					A.insert(3*i + 1, 3*j + 1) = row[j];
					A.insert(3*i + 2, 3*j + 2) = row[j];
				}
				A.coeffRef(3*i    , 3*i    ) = -1.0f;
				A.coeffRef(3*i + 1, 3*i + 1) = -1.0f;
				A.coeffRef(3*i + 2, 3*i + 2) = -1.0f;
			}
		}
		A.makeCompressed();

		/* build independent term vector */
		SparseMatrixf b(3*N, 1);
		for (int i = 0; i < 3*N; ++i) {
			b.insert(i, 0) = 0.0f;
		}
		b.makeCompressed();

		/* solve system of linear equations */
		Eigen::SimplicialLDLT<SparseMatrixf> SimpChol;
		SimpChol.compute(A);
		SparseMatrixf x = SimpChol.solve(b);

		/* check existence of solution and store */
		if (SimpChol.info() != Eigen::Success) {
			cerr << "No solution was found." << endl;
			return false;
		}

		vector<float> coords(3*N, 0.0f);
		for (int i = 0; i < 3*N; ++i) {
			coords[i] = x.coeff(i, 0);
		}
		m.set_vertices(coords);

		return true;
	}

	bool partial_smooth
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

		cout << "    Computing weights..." << endl;

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

		cout << "    Building system matrix..." << endl;

		/* build system matrix */
		SparseMatrixf A(3*variable, 3*variable);
		int row_it = 0;

		for (int _i = 0; _i < N; ++_i) {
			// skip constant vertices
			if (constant[_i]) { continue; }

			int col_it = 0;
			for (int _j = 0; _j < N; ++_j) {
				// skip constant vertices
				if (constant[_j]) { continue; }

				// assign weight of non-constant vertices
				A.insert(3*row_it    , 3*col_it    ) = ws[_i][_j];
				A.insert(3*row_it + 1, 3*col_it + 1) = ws[_i][_j];
				A.insert(3*row_it + 2, 3*col_it + 2) = ws[_i][_j];

				++col_it;
			}

			++row_it;
		}
		A.makeCompressed();

		cout << "    Building independent term vector..." << endl;

		/* build independent term vector */
		SparseMatrixf b(3*variable, 1);
		row_it = 0;
		for (int i = 0; i < N; ++i) {
			// skip constant vertices
			if (constant[i]) { continue; }

			float sum = 0.0f;
			for (int j = 0; j < N; ++j) {
				if (constant[j]) {
					sum -= ws[i][j];
				}
			}

			b.insert(3*row_it    , 0) = sum*m.get_vertex(row_it).x;
			b.insert(3*row_it + 1, 0) = sum*m.get_vertex(row_it).y;
			b.insert(3*row_it + 2, 0) = sum*m.get_vertex(row_it).z;

			++row_it;
		}
		b.makeCompressed();

		cout << "    Solving system..." << endl;

		/* solve system of linear equations */
		Eigen::SimplicialLDLT<SparseMatrixf> SimpChol;
		SimpChol.compute(A);
		SparseMatrixf x = SimpChol.solve(b);

		/* check existence of solution and store */
		if (SimpChol.info() != Eigen::Success) {
			cerr << "No solution was found." << endl;
			return false;
		}

		cout << "    Setting result..." << endl;

		vector<float> coords(3*N, 0.0f);
		int coord_it = 0;
		int sol_it = 0;
		for (int i = 0; i < N; ++i, coord_it += 3) {
			if (constant[i]) {
				coords[coord_it    ] = m.get_vertex(i).x;
				coords[coord_it + 1] = m.get_vertex(i).y;
				coords[coord_it + 2] = m.get_vertex(i).z;
			}
			else {
				coords[coord_it    ] = x.coeff(sol_it    , 0);
				coords[coord_it + 1] = x.coeff(sol_it + 1, 0);
				coords[coord_it + 2] = x.coeff(sol_it + 2, 0);
				sol_it += 3;
			}
		}
		m.set_vertices(coords);

		return true;
	}


} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
