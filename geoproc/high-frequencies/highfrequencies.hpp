#pragma once

// C++ includes
#include <stddef.h>
#include <vector>

// geoproc includes
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace high_frequencies {

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
	 * @brief Applies the difference of mesh and a smoothed version of it.
	 *
	 * Given a triangular mesh @e M, a configuration for a smoothing
	 * algorithm @e S, apply the operation:
	 *
	 * \f$M - S(M)\f$
	 *
	 * where \f$S(M)\f$ is the mesh smoothed using the configuration @e S.
	 * @param C Configuration of the smoothing operation.
	 * @param[out] m Mesh to be smoothed.
	 */
	void high_frequency_details(const smoothing_configuration& C, TriangleMesh& m);
	/**
	 * @brief Applies the difference of mesh and a smoothed version of it.
	 *
	 * Given a triangular mesh @e M, a configuration for a smoothing
	 * algorithm @e S, apply the operation:
	 *
	 * \f$M - S(M)\f$
	 *
	 * where \f$S(M)\f$ is the mesh smoothed using the configuration @e S.
	 * @param C Configuration of the smoothing operation.
	 * @param nt Number of threads.
	 * @param[out] m Mesh to be smoothed.
	 */
	void high_frequency_details(const smoothing_configuration& C, size_t nt, TriangleMesh& m);

	/**
	 * @brief Applies the difference of mesh and a smoothed version of it.
	 *
	 * Given a triangular mesh @e M, a configuration for a smoothing
	 * algorithm @e S, apply the operation:
	 *
	 * \f$M + \lambda(M - S(M))\f$
	 *
	 * where \f$S(M)\f$ is the mesh smoothed using the configuration @e S.
	 * @param C Configuration of the smoothing operation.
	 * @param[out] m Mesh to be smoothed.
	 */
	void exaggerate_high_frequencies(const smoothing_configuration& C, TriangleMesh& m);
	/**
	 * @brief Applies the difference of mesh and a smoothed version of it.
	 *
	 * Given a triangular mesh @e M, a configuration for a smoothing
	 * algorithm @e S, apply the operation:
	 *
	 * \f$M + \lambda(M - S(M))\f$
	 *
	 * where \f$S(M)\f$ is the mesh smoothed using the configuration @e S.
	 * @param C Configuration of the smoothing operation.
	 * @param nt Number of threads.
	 * @param[out] m Mesh to be smoothed.
	 */
	void exaggerate_high_frequencies(const smoothing_configuration& C, size_t nt, TriangleMesh& m);

	/**
	 * @brief Band frequencies operator.
	 *
	 * Apply several smoothing operations onto the mesh @e m. Each operation
	 * is described by a @ref smoothing_configuration struct.
	 *
	 * The operations applied are:
	 *
	 * \f$M + \sum_{i=1}^n \lambda_i \cdot D_i\f$
	 *
	 * where \f$D_i\f$ is the difference of meshes resulting from two consecutive
	 * smoothing operations, and \f$lambda_i\f$ are the values of \f$\lambda\f$
	 * in each configuration.
	 *
	 * The \f$D_i\f$ are more precisely defined as:
	 *
	 * \f$D_i = S_i(M) - S_{i+1}(M)\f$, where \f$S_i\f$ is the @e i-th smoothing
	 * configuration in parameter @e confs.
	 * @param confs The different configurations to be applied.
	 * @param[out] m The mesh resulting of applying the operations described above.
	 */
	void band_frequencies
	(const std::vector<smoothing_configuration>& confs, TriangleMesh& m);
	/**
	 * @brief Band frequencies operator.
	 *
	 * Apply several smoothing operations onto the mesh @e m. Each operation
	 * is described by a @ref smoothing_configuration struct.
	 *
	 * The operations applied are:
	 *
	 * \f$M + \sum_{i=1}^n \lambda_i \cdot D_i\f$
	 *
	 * where \f$D_i\f$ is the difference of meshes resulting from two consecutive
	 * smoothing operations, and \f$lambda_i\f$ are the values of \f$\lambda\f$
	 * in each configuration.
	 *
	 * The \f$D_i\f$ are more precisely defined as:
	 *
	 * \f$D_i = S_i(M) - S_{i+1}(M)\f$, where \f$S_i\f$ is the @e i-th smoothing
	 * configuration in parameter @e confs.
	 * @param confs The different configurations to be applied.
	 * @param nt Number of threads.
	 * @param[out] m The mesh resulting of applying the operations described above.
	 */
	void band_frequencies
	(const std::vector<smoothing_configuration>& confs, size_t nt, TriangleMesh& m);

} // -- namespace high_frequencies
} // -- namespace geoproc
