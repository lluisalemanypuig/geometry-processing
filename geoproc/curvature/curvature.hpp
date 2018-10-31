#pragma once

// C++ includes
#include <vector>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>

namespace algorithms {
namespace curvature {

	/**
	 * @brief Computes the Gauss curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref Gauss(const TriangleMesh&, std::vector<float>&) is called.
	 * @param mesh Input mesh.
	 * @param[out] Kg An approximation of the Gauss curvature per vertex.
	 */
	void Gauss(const TriangleMesh& mesh, std::vector<float>& Kg);

	/**
	 * @brief Computes the Gauss curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref Gauss(const TriangleMesh&, std::vector<float>&) is called.
	 * @param mesh Input mesh.
	 * @param[out] Kg An approximation of the Gauss curvature per vertex.
	 * @param n_threads Number of threads.
	 */
	void Gauss(const TriangleMesh& mesh, std::vector<float>& Kg, size_t n_threads);

	/**
	 * @brief Computes the mean curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 * @param mesh Input mesh.
	 * @param[out] Kh An approximation of the mean curvature per vertex.
	 */
	void mean(const TriangleMesh& mesh, std::vector<float>& Kh);
	/**
	 * @brief Computes the mean curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref mean(const TriangleMesh&, std::vector<float>&) is called.
	 * @param mesh Input mesh.
	 * @param[out] Kh An approximation of the mean curvature per vertex.
	 * @param n_threads Number of threads.
	 */
	void mean(const TriangleMesh& mesh, std::vector<float>& Kh, size_t n_threads);

} // -- namespace curavture
} // -- namespace algorithms
