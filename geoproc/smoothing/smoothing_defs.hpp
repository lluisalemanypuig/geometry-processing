#pragma once

// C++ includes
#include <cstdint>

namespace geoproc {
namespace smoothing {

	/**
	 * @brief The different types of operators available.
	 *
	 * - Laplacian: see @ref smooth_operator::Laplacian.
	 * - Bi-Laplacian: see @ref smooth_operator::BiLaplacian.
	 * - TaubinLM: see @ref smooth_operator::TaubinLM.
	 */
	enum class smooth_operator : int8_t {
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
	 * @brief The different types of weights for smoothing.
	 *
	 * - uniform: see @ref smooth_weight::uniform.
	 * - cotangent: see @ref smooth_weight::cotangent.
	 */
	enum class smooth_weight : int8_t {
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

} // -- namespace smoothing
} // -- namespace geoproc
