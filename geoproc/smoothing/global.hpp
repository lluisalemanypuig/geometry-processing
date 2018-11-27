#pragma once

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>

namespace geoproc {
namespace smoothing {
namespace global {

	/**
	 * @brief Apply a global smoothing algorithm for a given operator and weight.
	 *
	 * This algorithm smoothes a given mesh globally, and in one single step.
	 * It defines a system of linear equations, whose matrix is \f$A\f$. Such
	 * matrix is defined as
	 * - \f$A_{ii} = -1\f$
	 * - \f$A_{ij} = w_{ij}, \forall i \neq j\f$
	 * where \f$ 0 \le i,j \le N\f$.
	 *
	 * The values \f$w_{ij}\f$ are the weights for vertex \f$v_i\f$, and their
	 * values depend on parameter @e w. Matrix \f$A\f$ has dimensions
	 * \f$3N \times 3N\f$, where \f$N\f$ is the number of vertices of the mesh.
	 *
	 * For each vertex there is a \f$3 \times 3\f$ submatrix in \f$A\f$: the
	 * values corresponding to vertex \f$v_i\f$ are found in the submatrix
	 * \f$A_{[3i,3i+3), [3i,3i+3)}\f$, and the notation \f$A_{ij}=k\f$ actually
	 * means:
	 *
	 *	\f$ A_{[3i,3i+3), [3j,3j+3)} =
			\left( {\begin{array}{ccc}
				k & 0 & 0 \\
				0 & k & 0 \\
				0 & 0 & k \\
			\end{array} } \right)
		\f$
	 *
	 * The new coordinates of the vertices of the mesh is the solution \f$x\f$
	 * of the equation:
	 *
	 * \f$Ax = b\f$
	 *
	 * where \f$b\f$ contains some values. For no fixed vertices, this vector will
	 * be the zero vector. However, this makes the vertices collapse to the point
	 * (0,0,0). Therefore, this algorithm needs some vertices to be set constant
	 * (parameter @e constant).
	 *
	 * The vertices defined as constant are moved to the right hand-side of the
	 * equation and the matrix of the system becomes \f$A'\f$ and has dimensions
	 * \f$3v \times 3v\f$ where \f$v\f$ is the number of variable vertices.
	 *
	 * The system is solved using the Simplicial Cholesky method from library
	 * Eigen.
	 * @param op Smoothing operator.
	 * @param w Weight type.
	 * @param constant @e constant[i] is true if, and only if, @e i-th vertex
	 * is a constant vertex.
	 * @param[out] m Smoothed mesh.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 * @pre This algorithm is implemented for the following smoothing operators:
	 * - @ref smooth_operator::Laplacian
	 * @pre This algorithm is implemented for the following weight types:
	 * - @ref smooth_weight::uniform
	 * - @ref smooth_weight::cotangent
	 * @pre The last precodintion is that the mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 * @returns Returns true on success: parameters were correct and the system
	 * was solved succesfully.
	 */
	bool smooth
	(const smooth_operator& op, const smooth_weight& w,
	 const std::vector<bool>& constant, TriangleMesh& m);

} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
