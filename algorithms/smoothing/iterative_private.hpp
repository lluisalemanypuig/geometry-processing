#pragma once

// glm includes
#include <glm/glm.hpp>

// algorithms includes
#include <algorithms/triangle_mesh.hpp>
#include <algorithms/iterators/mesh_iterator.hpp>
#include <algorithms/iterators/vertex_iterators.hpp>
#include <algorithms/smoothing/smoothing_defs.hpp>

namespace algorithms {
namespace smoothing {
namespace smoothing_private {

	/**
	 * @brief Computes the sum corresponding to uniform weights.
	 *
	 * Computes the sum
	 *
	 * \f$L(v_i) = \sum_{j} w_{ij}\cdot(v_j - v_i)\f$
	 *
	 * where \f$w_{ij}\f$ correpond to uniform weights. See
	 * @ref smooth_weight::uniform. Recall that \f$j\f$ iterates
	 * over the 1-ring neighbourhood of @e vi.
	 * @param vi Valid vertex index.
	 * @param m Mesh to be iterated.
	 * @param verts Use the coordinates of the vertices in this
	 * vector instead of the vertices in the mesh.
	 * @param[out] L Result of the sum.
	 */
	void make_uniform_weight
	(int vi, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L);

	/**
	 * @brief Computes the sum corresponding to cotangent weights.
	 *
	 * Computes the sum
	 *
	 * \f$L(v_i) = \sum_{j} w_{ij}\cdot(v_j - v_i)\f$
	 *
	 * where \f$w_{ij}\f$ correpond to cotangent weights. See
	 * @ref smooth_weight::cotangent. Recall that \f$j\f$ iterates
	 * over the 1-ring neighbourhood of @e vi.
	 * @param vi Valid vertex index.
	 * @param m Mesh to be iterated.
	 * @param verts Use the coordinates of the vertices in this
	 * vector instead of the vertices in the mesh.
	 * @param[out] L Result of the sum.
	 */
	void make_cotangent_weight
	(int i, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L);

	/**
	 * @brief Applies one iteration of a local smoothing algorithm.
	 * @param w Type of smoothing weight.
	 * @param lambda Parameter used in moving vertices:
	 * \f$v_i' = v_i + \lambda\cdot L(v_i)\f$, where \f$L(v_i)\f$
	 * depends on the type of weight used.
	 * @param m The mesh used to iterate on the neighbourhood of @e old_verts.
	 * @param old_verts Coordinates of the vertices to be smoothed, whose
	 * neighbourhood relationship is encoded in the mesh @e m.
	 * @param[out] new_verts The coordinates of the smoothed vertices.
	 */
	void apply_local
	(
		const smooth_weight& w, float lambda,
		const TriangleMesh& m,
		const glm::vec3 *old_verts,
		glm::vec3 *new_verts
	);

} // -- namespace smoothing_private
} // -- namespace smoothing
} // -- namespace algorithms
