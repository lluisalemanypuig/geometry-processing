#pragma once

namespace algorithms {
namespace smoothing {

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
		 * \f$ \omega_{ij} = \cotan(\alpha) + \cotan(\beta) \f$
		 */
		cotangent
	};

} // -- namespace smoothing
} // -- namespace algorithms
