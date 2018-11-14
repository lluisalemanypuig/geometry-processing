#pragma once

// C++ includes
#include <stddef.h>
#include <vector>

// geoproc includes
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace filter_frequencies {

	/**
	 * @brief A smoothing algorithm configuration.
	 *
	 * Most smoothing algorithms require the same parameters. So, in order
	 * to easily configure the Band Frequencies obtention, this struct provides
	 * all the information necessary for it.
	 *
	 * It contains,
	 * - The operator used (see @ref so).
	 * - The type of weight applied (see @ref sw).
	 * - Value of parameter \f$\lambda\f$ (see @ref lambda).
	 * - Amount of iterations for the algorithm (see @ref n_it).
	 */
	struct smoothing_configuration {
		/**
		 * @brief Type of operator applied the algorithm uses.
		 *
		 * See @ref smoothing::smooth_operator.
		 */
		smoothing::smooth_operator so;
		/**
		 * @brief Type of weight applied within the operator.
		 *
		 * See @ref smoothing::smooth_weight.
		 */
		smoothing::smooth_weight sw;
		/// Value of parameter \f$\lambda\f$.
		float lambda;
		/// Amount of iterations of algorithm.
		size_t n_it;
	};

	/**
	 * @brief Band frequencies operator.
	 *
	 * Apply several smoothing operations onto the mesh @e m. Each operation
	 * is described by a @ref smoothing_configuration struct.
	 *
	 * The operations applied are:
	 *
	 * \f$M + \sum_{i=1}^n \mu_i \cdot D_i\f$
	 *
	 * where each \f$D_i\f$ is the difference of meshes resulting from two
	 * consecutive smoothing operations (or band frequency), and \f$mu_i\f$
	 * are the weights for each band frequency.
	 *
	 * The band frequencies \f$D_i\f$ are more precisely defined as:
	 *
	 * \f$D_i = S_i(M) - S_{i+1}(M)\f$, where \f$S_i\f$ is the @e i-th smoothing
	 * configuration in parameter @e confs. Notice, then, that there are
	 * \f$n + 1\f$ smoothing configurations in @e confs.
	 *
	 * @param confs The \f$ n + 1\f$ configurations to be applied.
	 * @param mus The weights \f$\mu_1,\cdots,\mu_n\f$ for each band frequency.
	 * @param[out] m The mesh resulting of applying the operations described above.
	 */
	void band_frequencies
	(const std::vector<smoothing_configuration>& confs,
	 const std::vector<float>& mus, TriangleMesh& m);
	/**
	 * @brief Band frequencies operator.
	 *
	 * Apply several smoothing operations onto the mesh @e m. Each operation
	 * is described by a @ref smoothing_configuration struct.
	 *
	 * The operations applied are:
	 *
	 * \f$M + \sum_{i=1}^n \mu_i \cdot D_i\f$
	 *
	 * where each \f$D_i\f$ is the difference of meshes resulting from two
	 * consecutive smoothing operations (or band frequency), and \f$mu_i\f$
	 * are the weights for each band frequency.
	 *
	 * The band frequencies \f$D_i\f$ are more precisely defined as:
	 *
	 * \f$D_i = S_i(M) - S_{i+1}(M)\f$, where \f$S_i\f$ is the @e i-th smoothing
	 * configuration in parameter @e confs. Notice, then, that there are
	 * \f$n + 1\f$ smoothing configurations in @e confs.
	 *
	 * @param confs The \f$ n + 1\f$ configurations to be applied.
	 * @param mus The weights \f$\mu_1,\cdots,\mu_n\f$ for each band frequency.
	 * @param nt Number of threads.
	 * @param[out] m The mesh resulting of applying the operations described above.
	 */
	void band_frequencies
	(const std::vector<smoothing_configuration>& confs,
	 const std::vector<float>& mus, size_t nt, TriangleMesh& m);

} // -- namespace filter_frequencies
} // -- namespace geoproc
