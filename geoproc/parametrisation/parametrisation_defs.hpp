#pragma once

// C includes
#include <inttypes.h>

namespace geoproc {
namespace parametrisation {

	/**
	 * @brief Shapes of the boundary vertices on the texture.
	 *
	 * - circle: see @ref boundary_shape::Circle.
	 * - square: see @ref boundary_shape::Square.
	 */
	enum class boundary_shape : int8_t {
		/// The boundary vertices are placed on the circumference of
		/// a circle of radius 1, centered at (0,0).
		Circle = 0,
		/// The boundary vertices are placed on the boundary of
		/// a square of side length 2, centered at (0,0).
		Square
	};

} // -- namespace parametrisation
} // -- namespace geoproc
