#pragma once

// C++ includes
#include <cstdint>

namespace geoproc {

/**
 * @brief The different types of operators available.
 *
 * - Laplacian: see @ref smooth_modifier::Laplacian.
 * - Bi-Laplacian: see @ref smooth_modifier::BiLaplacian.
 * - TaubinLM: see @ref smooth_modifier::TaubinLM.
 */
enum class modifier : int8_t {
	/**
	 * Detailed in the documentation of functions:
	 * - @ref local::laplacian(const smooth_weight&, float, size_t, TriangleMesh&)
	 * - @ref local::laplacian(const smooth_weight&, float, size_t, size_t, TriangleMesh&)
	 */
	Laplacian,
	/**
	 * Detailed in the documentation of functions:
	 * - @ref local::bilaplacian(const smooth_weight&, float, size_t, TriangleMesh&)
	 * - @ref local::bilaplacian(const smooth_weight&, float, size_t, size_t, TriangleMesh&)
	 */
	BiLaplacian,
	/**
	 * Detailed in the documentation of functions:
	 * - @ref local::TaubinLM(const smooth_weight&, float, size_t, TriangleMesh&)
	 * - @ref local::TaubinLM(const smooth_weight&, float, size_t, size_t, TriangleMesh&)
	 */
	TaubinLM
};

/**
 * @brief The different types of weights.
 *
 * - uniform: see @ref smooth_weight::uniform.
 * - cotangent: see @ref smooth_weight::cotangent.
 */
enum class weight : int8_t {
	/**
	 * @brief Uniform weights.
	 *
	 * For a vertex \f$v_i\f$, the contribution of each vertex of its
	 * neighbours \f$v_j\f$ is weighted uniformly.
	 *
	 * That is, if vertex \f$v_i\f$ has \f$N\f$ neighbours, the weights
	 * are defined as:
	 *
	 * \f$ \omega_{ij} = \frac{1}{N} \f$
	 */
	uniform = 0,
	/**
	 * @brief Cotangent weights.
	 *
	 * For a vertex \f$v_i\f$, the contribution of each vertex of its
	 * neighbours \f$v_j\f$ is weighted using the sum of the cotangents
	 * of the angles \f$ v_i,v_k,v_j \f$ and .\f$ v_i,v_l,v_j \f$,
	 * where \f$ v_k,v_l \f$ are the two common neighbours of \f$ v_i,v_j \f$.
	 *
	 * That is, if \f$ \alpha = \langle v_i,v_k,v_j \rangle \f$, and
	 * \f$ \beta = \langle v_i,v_l,v_j \rangle \f$ are the two aforementioned
	 * angles, then the weight \f$ \omega_{ij} \f$ is defined as:
	 *
	 * \f$ \omega_{ij} = \cot(\alpha) + \cot(\beta) \f$
	 */
	cotangent
};

/**
 * @brief Shapes for boundary vertices.
 *
 * Sometimes we may want to place the vertices at the boundary of a mesh
 * in a certain shape on the xy plane. The shapes are the following:
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

} // -- namespace geoproc
