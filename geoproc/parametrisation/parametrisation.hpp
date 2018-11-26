#pragma once

// C++ includes
#include <vector>

// glm includes
#include <glm/vec3.hpp>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace parametrisation {

	/**
	 * @brief Shapes of the boundary vertices on the texture.
	 *
	 * - circle: see @ref Circle.
	 * - square: see @ref Square.
	 */
	enum class boundary_shape : int8_t {
		/// The boundary vertices are placed on the circumference of
		/// a circle of radius 1, centered at (0,0).
		Circle,
		/// The boundary vertices are placed on the boundary of
		/// a square of side length 1, centered at (0,0).
		Square
	};

	/**
	 * @brief Calculates the texture coordinates using Harmonic Maps
	 * @param[in] m Input mesh. It must have one single boundary.
	 * @param[in] s Shape of the boundary vertices on the texture.
	 * @param[out] uvs Texture coordinates of the vertices of @e m.
	 */
	void harmonic_map
	(const TriangleMesh& m, const boundary_shape& s, std::vector<glm::vec2>& uvs);

} // -- namespace parametrisation
} // -- namespace geoproc
