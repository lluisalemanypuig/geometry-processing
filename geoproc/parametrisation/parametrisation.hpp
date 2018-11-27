#pragma once

// C++ includes
#include <vector>

// glm includes
#include <glm/vec3.hpp>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>

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
	 * @param m Input mesh. It must have one single boundary.
	 * @param w Type of weights to use to compute the parametrisation.
	 * @param s Shape of the boundary vertices on the texture.
	 * @param[out] uvs Texture coordinates of the vertices of @e m.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Boundaries (see @ref TriangleMesh::make_boundaries)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 * @return Returns true on success. The procedure might fail if there
	 * are no boundaries on the mesh or there are more than one.
	 */
	bool harmonic_maps
	(const TriangleMesh& m, const smoothing::smooth_weight& w,
	 const boundary_shape& s, std::vector<glm::vec2>& uvs);

} // -- namespace parametrisation
} // -- namespace geoproc
