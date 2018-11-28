#include <geoproc/parametrisation/parametrisation.hpp>

// C++ includes
#include <iostream>
using namespace std;

// geoproc includes
#include <geoproc/smoothing/local_private.hpp>

// Eigen includes
#include <Eigen/Sparse>

namespace geoproc {
using namespace smoothing;
namespace parametrisation {

	typedef Eigen::Triplet<float> T;
	typedef Eigen::MatrixXf Matrixf;
	typedef Eigen::VectorXf Vectorf;
	typedef Eigen::SparseMatrix<float> SparseMatrixf;

	bool harmonic_maps
	(
		const TriangleMesh& m, const smooth_weight& w,
		const boundary_shape& s, vector<glm::vec2>& uvs
	)
	{
		const vector<vector<int> >& bounds = m.get_boundaries();
		if (bounds.size() == 0) {
			cerr << "geoproc::parametrisation::harmonic_maps - Error" << endl;
			cerr << "    No boundaries in input mesh." << endl;
			return false;
		}
		if (bounds.size() > 1) {
			cerr << "geoproc::parametrisation::harmonic_maps - Error" << endl;
			cerr << "    Too many boundaries in input mesh." << endl;
			return false;
		}

		const int N = m.n_vertices();
		uvs = vector<glm::vec2>(N, glm::vec2(0.0f,0.0f));

		// constant vertices
		vector<bool> constant(N, false);
		// boundary contains corner indices
		const vector<int>& boundary = bounds[0];

		if (s == boundary_shape::Circle) {
			// place vertices of the boundary on a circle of radius 1,
			// starting at (1,0), in counterclockwise order

			// angle increment
			float inc = (2.0f*M_PI)/boundary.size();
			float alpha = 0.0f;
			for (size_t i = 0; i < boundary.size(); ++i, alpha += inc) {
				int v_idx = m.get_vertex_corner(boundary[i]);
				constant[v_idx] = true;
				uvs[v_idx].x = std::cos(alpha);
				uvs[v_idx].y = std::sin(alpha);
			}
		}
		else if (s == boundary_shape::Square) {
			// place vertices of the boundary on a square of side
			// length 1, starting at (1,-1), in counterclockwise order
			int r = boundary.size()%4;
			int n_side1 = boundary.size()/4 + (r > 0 ? 1 : 0); --r;
			int n_side2 = boundary.size()/4 + (r > 0 ? 1 : 0); --r;
			int n_side3 = boundary.size()/4 + (r > 0 ? 1 : 0); --r;
			int n_side4 = boundary.size()/4 + (r > 0 ? 1 : 0);

			// texture coordinates of boundary vertices
			size_t bound_it = 0;
			for (int i = 0; i < n_side1; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				constant[v_idx] = true;
				uvs[v_idx].x = 1.0f;
				uvs[v_idx].y = -1.0f + i*(2.0f/n_side1);
			}
			for (int i = 0; i < n_side2; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				constant[v_idx] = true;
				uvs[v_idx].x = 1.0f - i*(2.0f/n_side2);
				uvs[v_idx].y = 1.0f;
			}
			for (int i = 0; i < n_side3; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				constant[v_idx] = true;
				uvs[v_idx].x = -1.0f;
				uvs[v_idx].y = 1.0f - i*(2.0f/n_side3);
			}
			for (int i = 0; i < n_side4; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				constant[v_idx] = true;
				uvs[v_idx].x = -1.0f + i*(2.0f/n_side4);
				uvs[v_idx].y = -1.0f;
			}
		}

		const int n_constant = boundary.size();
		const int variable = N - n_constant;

		// weights for system's matrix
		vector<vector<float> > ws(N, vector<float>(N, 0.0f));
		if (w == smooth_weight::uniform) {
			for (int i = 0; i < N; ++i) {
				local_private::make_uniform_weights(i, m, &ws[i][0]);
				ws[i][i] = -1.0f;
			}
		}
		else if (w == smooth_weight::cotangent) {
			for (int i = 0; i < N; ++i) {
				local_private::make_cotangent_weights(i, m, &ws[i][0]);
				ws[i][i] = -1.0f;
			}
		}

		vector<T> triplet_list;

		/* reserve independent term vector */
		Eigen::VectorXf bX(variable), bY(variable);

		/* fill independent term vector
		   and compute triplets for system matrix */

		int row_it = 0;
		for (int i = 0; i < N; ++i) {
			// skip rows of constant vertices
			if (constant[i]) { continue; }

			int col_it = 0;
			glm::vec2 sums(0.0f,0.0f);
			for (int j = 0; j < N; ++j) {
				if (constant[j]) {
					// if the vertex at the j-th column is constant
					// we need to accumulate the sum
					sums += ws[i][j]*uvs[j];
				}
				else {
					// if it is not constant we have a
					// triplet of the system matrix
					if (ws[i][j] != 0.0f) {
						triplet_list.push_back(T(row_it, col_it, ws[i][j]));
					}
					++col_it;
				}
			}

			bX(row_it) = -sums.x;
			bY(row_it) = -sums.y;
			++row_it;
		}

		ws.clear();

		#if defined (DEBUG)
		cout << "    Building system matrix..." << endl;
		#endif

		/* build system matrix */
		SparseMatrixf A(variable, variable);
		A.setFromTriplets(triplet_list.begin(), triplet_list.end());
		triplet_list.clear();
		A.makeCompressed();

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
		cout << "        x" << endl;
		#endif

		Vectorf solX = solver.solve(At*bX);

		#if defined (DEBUG)
		cout << "        y" << endl;
		#endif

		Vectorf solY = solver.solve(At*bY);

		int fixed_it = 0;
		for (int i = 0; i < N; ++i) {
			if (not constant[i]) {
				uvs[i].x = solX(fixed_it);
				uvs[i].y = solY(fixed_it);
				++fixed_it;
			}
		}

		return true;
	}

} // -- namespace parametrisation
} // -- namespace geoproc
