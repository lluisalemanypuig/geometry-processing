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
	 * \f$N \times N\f$, where \f$N\f$ is the number of vertices of the mesh.
	 * (keep reading to know how fixed vertices are considered...).
	 *
	 * The new coordinates of the vertices of the mesh is the solution \f$x\f$
	 * of the equation:
	 *
	 * \f$Ax_x = b_x\f$, \f$Ax_y = b_y\f$, \f$Ax_z = b_z\f$
	 *
	 * where \f$b_x,b_y,b_z\f$ contain some values. When parameter @e constant
	 * contains no fixed vertices, these vectors will be the zero vector. However,
	 * this makes the vertices collapse to the point (0,0,0). Therefore, this
	 * algorithm needs some vertices to be set constant (parameter @e constant).
	 *
	 * The vertices defined as constant are moved to the right hand-side of the
	 * equation and the matrix of the system becomes \f$A'\f$ and has dimensions
	 * \f$v \times v\f$ where \f$v\f$ is the number of variable vertices:
	 *
	 * \f$v = N - v^*\f$, where \f$v^*\f$ is the amount of fixed vertices in
	 * @e constant.
	 *
	 * The systems are solved using the Simplicial Cholesky method from library
	 * Eigen in a least square sense:
	 *
	 * \f$ A^TAx_x = A^Tb_x\f$, \f$A^TAx_y = A^Tb_y\f$, \f$A^TAx_z = A^Tb_z\f$
	 *
	 * In order to save time, the decomposition needed for the Cholesky
	 * method is computed only once and reused to solve the three systems
	 * defined above.
	 *
	 * Now, the Laplacian operator defines the linear equation for vertex \f$v_i\f$
	 * as:
	 *
	 * \f$L(v_i) = \left(\sum_{j \in N(i)} w_{ij}*v_j\right) - v_i\f$
	 *
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

	/**
	 * @brief Apply a global smoothing algorithm for a given operator and weight.
	 *
	 * This procedure does the same as
	 * @ref smooth(const smooth_operator&, const smooth_weight&, const std::vector<bool>&, TriangleMesh&)
	 * but uses as many threads as indicated in parameter @e n_threads.
	 * @param op Smoothing operator.
	 * @param w Weight type.
	 * @param constant @e constant[i] is true if, and only if, @e i-th vertex
	 * is a constant vertex.
	 * @param n_threads Number of threads.
	 * @param[out] m Smoothed mesh.
	 * @pre The same preconditions as
	 * @ref smooth(const smooth_operator&, const smooth_weight&, const std::vector<bool>&, TriangleMesh&);
	 * @returns Returns true on success: parameters were correct and the system
	 * was solved succesfully.
	 */
	bool smooth
	(const smooth_operator& op, const smooth_weight& w,
	 const std::vector<bool>& constant, size_t n_threads, TriangleMesh& m);

} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
