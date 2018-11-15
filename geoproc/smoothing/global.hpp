#pragma once

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>

namespace geoproc {
namespace smoothing {
namespace global {

	/**
	 * @brief Use a global smoothing algorithm for a given operator and weight.
	 *
	 * The system is defined for all vertices of the mesh.
	 * @param op Smoothing operator.
	 * @param w Weight type.
	 * @param[out] m Smoothed mesh.
	 * @pre This algorithm is implemented for the following smoothing
	 * operators:
	 * - @ref smooth_operator::Laplacian
	 *
	 * This algorithm is implemented for the following weight types:
	 * - @ref smooth_weight::uniform
	 * - @ref smooth_weight::cotangent
	 */
	void full_smooth
	(const smooth_operator& op, const smooth_weight& w, TriangleMesh& m);

} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
