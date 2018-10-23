#pragma once

// C includes
#include <omp.h>

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// Custom includes
#include <algorithms/triangle_mesh.hpp>
#include <algorithms/iterators/mesh_iterator.hpp>
#include <algorithms/iterators/vertex_iterators.hpp>

namespace algorithms {
namespace smoothing {

	/**
	 * @brief The different types of weights for smoothing algorithms.
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

	/**
	 * @brief Laplacian smoothing of a mesh.
	 *
	 * Applies the Laplacian operator on each vertex of the mesh so as to
	 * smooth it.
	 *
	 * Each vertex of the mesh is translated:
	 *
	 * \f$ v_i' = v_i + \lambda \cdot L(v_i) \f$
	 *
	 * where
	 *
	 * \f$ L(v_i) = \sum_{j} w_{ij}(v_j - v_i) \f$
	 *
	 * The sum iterates over the 1-ring of vertex \f$v_i\f$.
	 * The weights \f$ w_{ij} \f$ are determined by the parameter @e w
	 * and are normalised so that, for a given vertex \f$v_i\f$:
	 *
	 * \f$ \sum_{j} w_{ij} = 1 \f$,
	 *
	 * in other words:
	 *
	 * \f$ w_{ij} = \frac{ \omega_{ij} }{ \sum_{k} \omega_{ik} } \f$
	 *
	 * Again, the sums iterate over the 1-ring of vertex \f$v_i\f$.
	 * Here the \f$ \omega_{ij} \f$ are the weights determined by the
	 * parameter @e w.
	 *
	 * Finally, the vertices will be smoothed a number of iterations,
	 * determined by the parameter @e n_iter.
	 *
	 * @param w The type of weight used.
	 * @param lambda Smoothing parameter.
	 * @param n_iter Number of iterations of the algorithm.
	 * @param m Mesh to be smoothed.
	 */
	void laplacian(const smooth_weight& w, float lambda, size_t n_iter, TriangleMesh& m);

} // -- namespace smoothing
} // -- namespace algorithms
