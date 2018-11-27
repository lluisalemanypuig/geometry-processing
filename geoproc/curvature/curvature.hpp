#pragma once

// C++ includes
#include <vector>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace curvature {

	/**
	 * @brief Computes the Gauss curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * This function needs the meshe's angles and areas (see
	 * @ref TriangleMesh::make_angles_area).
	 * @param mesh Input mesh.
	 * @param[out] Kg An approximation of the Gauss curvature per vertex.
	 * @param[out] min Minimum curvature value.
	 * @param[out] max Maximum curvature value.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 */
	void Gauss
	(
		const TriangleMesh& mesh, std::vector<float>& Kg,
		float *min = nullptr, float *max = nullptr
	);

	/**
	 * @brief Computes the Gauss curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref Gauss(const TriangleMesh&, std::vector<float>&,float*,float*)
	 * is called.
	 *
	 * This function needs neighbourhood data and the meshe's angles
	 * and areas (see @ref TriangleMesh::make_angles_area).
	 * @param mesh Input mesh.
	 * @param[out] Kg An approximation of the Gauss curvature per vertex.
	 * @param n_threads Number of threads.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 */
	void Gauss(const TriangleMesh& mesh, std::vector<float>& Kg, size_t n_threads);
	/**
	 * @brief Computes the Gauss curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref Gauss(const TriangleMesh&, std::vector<float>&,float*,float*) is called.
	 *
	 * This function needs neighbourhood data and the meshe's angles
	 * and areas (see @ref TriangleMesh::make_angles_area).
	 * @param mesh Input mesh.
	 * @param n_threads Number of threads.
	 * @param[out] Kg An approximation of the Gauss curvature per vertex.
	 * @param[out] min Minimum curvature value.
	 * @param[out] max Maximum curvature value.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 */
	void Gauss
	(
		const TriangleMesh& mesh, std::vector<float>& Kg,
		size_t n_threads, float *min, float *max
	);

	/**
	 * @brief Computes the mean curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * This function needs neighbourhood data and the meshe's angles
	 * and areas (see @ref TriangleMesh::make_angles_area).
	 * @param mesh Input mesh.
	 * @param[out] Kh An approximation of the mean curvature per vertex.
	 * @param[out] min Minimum curvature value.
	 * @param[out] max Maximum curvature value.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 */
	void mean
	(
		const TriangleMesh& mesh, std::vector<float>& Kh,
		float *min = nullptr, float *max = nullptr
	);
	/**
	 * @brief Computes the mean curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref mean(const TriangleMesh&, std::vector<float>&,float*,float*)
	 * is called.
	 *
	 * This function needs neighbourhood data and the meshe's angles
	 * and areas (see @ref TriangleMesh::make_angles_area).
	 * @param mesh Input mesh.
	 * @param[out] Kh An approximation of the mean curvature per vertex.
	 * @param n_threads Number of threads.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 */
	void mean(const TriangleMesh& mesh, std::vector<float>& Kh, size_t n_threads);
	/**
	 * @brief Computes the mean curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 *
	 * If the number of threads given is 1 then
	 * @ref mean(const TriangleMesh&, std::vector<float>&,float*,float*)
	 * is called.
	 *
	 * This function needs neighbourhood data and the meshe's angles
	 * and areas (see @ref TriangleMesh::make_angles_area).
	 * @param mesh Input mesh.
	 * @param n_threads Number of threads.
	 * @param[out] Kh An approximation of the mean curvature per vertex.
	 * @param[out] min Minimum curvature value.
	 * @param[out] max Maximum curvature value.
	 * @pre The mesh requires:
	 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
	 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
	 */
	void mean
	(
		const TriangleMesh& mesh, std::vector<float>& Kh,
		size_t n_threads, float *min, float *max
	);

} // -- namespace curavture
} // -- namespace geoproc
