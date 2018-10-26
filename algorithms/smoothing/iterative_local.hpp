#pragma once

// C includes
#include <omp.h>

// glm includes
#include <glm/glm.hpp>

// Custom includes
#include <algorithms/triangle_mesh.hpp>
#include <algorithms/smoothing/smoothing_defs.hpp>
#include <algorithms/smoothing/smoothing_private.hpp>

namespace algorithms {
namespace smoothing {

	/**
	 * @brief Laplacian operator for mesh smoothing.
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
