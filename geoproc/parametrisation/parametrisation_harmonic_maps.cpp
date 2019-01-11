#include <geoproc/parametrisation/parametrisation.hpp>

// C++ includes
#include <iostream>
using namespace std;

// Eigen includes
#include <Eigen/Sparse>

// geoproc includes
#include <geoproc/smoothing/local_private.hpp>
#include <geoproc/definitions.hpp>

#define to_double(x) static_cast<double>(x)
#define to_float(x) static_cast<float>(x)

typedef Eigen::Triplet<float> Tf;
typedef Eigen::Triplet<double> Td;
typedef Eigen::MatrixXf Matrixf;
typedef Eigen::VectorXf Vectorf;
typedef Eigen::VectorXd Vectord;
typedef Eigen::SparseMatrix<float> SparseMatrixf;
typedef Eigen::SparseMatrix<double> SparseMatrixd;

inline void harmonic_maps_laplacian
(
	const geoproc::TriangleMesh& m,
	int N, int variable,
	const geoproc::weight& w,
	const std::vector<bool>& constant,
	vector<glm::vec2>& uvs
)
{
	/* This code was engineered to avoid high memory
	 * consumption (on a system with 8GB the old code
	 * would take up to 44% of the total memory: ~4GB).
	 */

	// system's matrix
	SparseMatrixd A(variable, variable);
	// independent term vectors
	Vectord bX(variable), bY(variable);

	// weights for system's matrix (per row)
	double *ws = static_cast<double *>(malloc(N*sizeof(double)));
	// list of triplets for system's matrix
	vector<Td> triplet_list;

	int row_it = 0;
	for (int i = 0; i < N; ++i) {
		if (constant[i]) { continue; }

		// compute weights
		if (w == geoproc::weight::uniform) {
			geoproc::smoothing::local_private::make_uniform_weights(i, m, ws);
		}
		else if (w == geoproc::weight::cotangent) {
			geoproc::smoothing::local_private::make_cotangent_weights(i, m, ws);
		}

		ws[i] = -1.0;

		// compute the right handside of the equation
		// (vectors bX,bY,bZ) using this row
		int col_it = 0;

		glm::vec2d sums(0.0f,0.0f);
		for (int j = 0; j < N; ++j) {
			if (constant[j]) {
				// if the vertex at the j-th column is constant
				// we need to accumulate the sum
				sums.x += ws[j]*to_double(uvs[j].x);
				sums.y += ws[j]*to_double(uvs[j].y);
			}
			else {
				// if it is not constant we have a
				// triplet of the system matrix
				if (ws[j] != 0.0) {
					triplet_list.push_back(Td(row_it, col_it, ws[j]));
				}
				++col_it;
			}
		}

		bX(row_it) = -sums.x;
		bY(row_it) = -sums.y;
		++row_it;
	}

	// free memory
	free(ws);

	A.setFromTriplets(triplet_list.begin(), triplet_list.end());
	// free more memory
	triplet_list.clear();
	// improve memory consumption
	A.makeCompressed();

	SparseMatrixd At = A.transpose();
	Eigen::SimplicialCholesky<SparseMatrixd> solver(At*A);
	Vectord solX = solver.solve(At*bX);
	Vectord solY = solver.solve(At*bY);

	int fixed_it = 0;
	for (size_t i = 0; i < static_cast<size_t>(N); ++i) {
		if (not constant[i]) {
			uvs[i].x = to_float(solX(fixed_it));
			uvs[i].y = to_float(solY(fixed_it));
			++fixed_it;
		}
	}
}

namespace geoproc {
using namespace smoothing;
namespace parametrisation {

bool harmonic_maps
(
	const TriangleMesh& m, const weight& w,
	const boundary_shape& s, std::vector<glm::vec2>& uvs
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

	const size_t N = static_cast<size_t>(m.n_vertices());
	uvs = vector<glm::vec2>(N, glm::vec2(-1.0f,-1.0f));

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
			size_t v_idx = static_cast<size_t>(boundary[i]);
			constant[v_idx] = true;
			uvs[v_idx].x = std::cos(alpha);
			uvs[v_idx].y = std::sin(alpha);
		}
	}
	else if (s == boundary_shape::Square) {
		// place vertices of the boundary on a square of side
		// length 1, starting at (1,-1), in counterclockwise order
		size_t r = boundary.size()%4;
		size_t n_side1 = boundary.size()/4 + size_t(r > 0); --r;
		size_t n_side2 = boundary.size()/4 + size_t(r > 0); --r;
		size_t n_side3 = boundary.size()/4 + size_t(r > 0); --r;
		size_t n_side4 = boundary.size()/4 + size_t(r > 0);

		// texture coordinates of boundary vertices
		size_t bound_it = 0;
		for (size_t i = 0; i < n_side1; ++i, ++bound_it) {
			size_t v_idx = static_cast<size_t>(boundary[bound_it]);
			constant[v_idx] = true;
			uvs[v_idx].x =  1.0f;
			uvs[v_idx].y = -1.0f + i*(2.0f/n_side1);
		}
		for (size_t i = 0; i < n_side2; ++i, ++bound_it) {
			size_t v_idx = static_cast<size_t>(boundary[bound_it]);
			constant[v_idx] = true;
			uvs[v_idx].x = 1.0f - i*(2.0f/n_side2);
			uvs[v_idx].y = 1.0f;
		}
		for (size_t i = 0; i < n_side3; ++i, ++bound_it) {
			size_t v_idx = static_cast<size_t>(boundary[bound_it]);
			constant[v_idx] = true;
			uvs[v_idx].x = -1.0f;
			uvs[v_idx].y =  1.0f - i*(2.0f/n_side3);
		}
		for (size_t i = 0; i < n_side4; ++i, ++bound_it) {
			size_t v_idx = static_cast<size_t>(boundary[bound_it]);
			constant[v_idx] = true;
			uvs[v_idx].x = -1.0f + i*(2.0f/n_side4);
			uvs[v_idx].y = -1.0f;
		}
	}

	const int n_constant = boundary.size();
	const int variable = N - n_constant;

	// compute uvs
	harmonic_maps_laplacian(m, N, variable, w, constant, uvs);

	// rescale to [0,0]x[1,1]
	for (size_t i = 0; i < uvs.size(); ++i) {
		uvs[i].x = 0.5f*uvs[i].x + 0.5f;
		uvs[i].y = 0.5f*uvs[i].y + 0.5f;
	}
	return true;
}

} // -- namespace parametrisation
} // -- namespace geoproc
