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
	 * Unlike @ref full_smooth, the matrix of the system is defined only for
	 * those non-fixed vertices, i.e., those vertices \e i such that
	 * @e constant[i] evaluates to false.
	 * @param op Smoothing operator.
	 * @param w Weight type.
	 * @param[out] m Smoothed mesh.
	 * @returns Returns true on success: parameters were correct and the system
	 * was solved succesfully.
	 * @pre This algorithm is implemented for the following smoothing
	 * operators:
	 * - @ref smooth_operator::Laplacian
	 *
	 * This algorithm is implemented for the following weight types:
	 * - @ref smooth_weight::uniform
	 * - @ref smooth_weight::cotangent
	 */
	bool smooth
	(const smooth_operator& op, const smooth_weight& w,
	 const std::vector<bool>& constant, TriangleMesh& m);

} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
