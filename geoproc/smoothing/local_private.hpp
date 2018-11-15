#pragma once

// glm includes
#include <glm/glm.hpp>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>

namespace geoproc {
namespace smoothing {
namespace local_private {

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
	(int vi, const TriangleMesh& m, const glm::vec3 *verts, glm::vec3& L);

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
	/**
	 * @brief Applies one parallelised iteration of a local smoothing algorithm.
	 * @param w Type of smoothing weight.
	 * @param lambda Parameter used in moving vertices:
	 * \f$v_i' = v_i + \lambda\cdot L(v_i)\f$, where \f$L(v_i)\f$
	 * depends on the type of weight used.
	 * @param m The mesh used to iterate on the neighbourhood of @e old_verts.
	 * @param nt Number of threads.
	 * @param old_verts Coordinates of the vertices to be smoothed, whose
	 * neighbourhood relationship is encoded in the mesh @e m.
	 * @param[out] new_verts The coordinates of the smoothed vertices.
	 */
	void apply_local
	(
		const smooth_weight& w, float lambda,
		const TriangleMesh& m,
		size_t nt,
		const glm::vec3 *old_verts,
		glm::vec3 *new_verts
	);

	/**
	 * @brief Applies a sequential local procedure once at every @e nit iterations.
	 * @param w Type of smoothing weight.
	 * @param l Parameter used in moving vertices:
	 * \f$v_i' = v_i + \lambda\cdot L(v_i)\f$, where \f$L(v_i)\f$
	 * depends on the type of weight used.
	 * @param nit Number of iterations of the algorithm.
	 * @param m The mesh used to iterate on the neighbourhood of @e old_verts.
	 * @param[out] old_verts Initially, the vertices of the mesh. At the end of
	 * algorithm they may contain the final coordinates of the vertices of the
	 * mesh depending on the returned value.
	 * @param[out] new_verts At the end of algorithm they may contain the final
	 * coordinates of the vertices of the mesh depending on the returned value.
	 * @return Returns true if the final coordinates of the vertices are in
	 * new_verts. Returns false if otherwise.
	 */
	bool apply_once_per_it
	(
		const smooth_weight& w, float l,
		size_t nit,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
	);
	/**
	 * @brief Applies a parallel local procedure once at every @e nit iterations.
	 * @param w Type of smoothing weight.
	 * @param l Parameter used in moving vertices:
	 * \f$v_i' = v_i + \lambda\cdot L(v_i)\f$, where \f$L(v_i)\f$
	 * depends on the type of weight used.
	 * @param nit Number of iterations of the algorithm.
	 * @param n_threads Number of threads.
	 * @param m The mesh used to iterate on the neighbourhood of @e old_verts.
	 * @param[out] old_verts Initially, the vertices of the mesh. At the end of
	 * algorithm they may contain the final coordinates of the vertices of the
	 * mesh depending on the returned value.
	 * @param[out] new_verts At the end of algorithm they may contain the final
	 * coordinates of the vertices of the mesh depending on the returned value.
	 * @return Returns true if the final coordinates of the vertices are in
	 * new_verts. Returns false if otherwise.
	 */
	bool apply_once_per_it
	(
		const smooth_weight& w, float l,
		size_t nit, size_t n_threads,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
	);

	/**
	 * @brief Applies two sequential local procedure at every @e nit iterations.
	 * @param w Type of smoothing weight.
	 * @param l1 Parameter used in moving vertices:
	 * \f$v_i' = v_i + \lambda_1\cdot L(v_i)\f$, where \f$L(v_i)\f$
	 * depends on the type of weight used.
	 * @param l2 Parameter used in moving vertices:
	 * \f$v_i'' = v_i' + \lambda_2\cdot L(v_i')\f$, where \f$L(v_i')\f$
	 * depends on the type of weight used.
	 * @param nit Number of iterations of the algorithm.
	 * @param m The mesh used to iterate on the neighbourhood of @e old_verts.
	 * @param[in] old_verts Initially, the vertices of the mesh.
	 * @param[out] new_verts At the end of algorithm they contain the final
	 * coordinates of the vertices of the mesh.
	 */
	void apply_twice_per_it
	(
		const smooth_weight& w,
		float l1, float l2,
		size_t nit,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
	);
	/**
	 * @brief Applies twice parallel local procedure at every @e nit iterations.
	 * @param w Type of smoothing weight.
	 * @param l1 Parameter used in moving vertices:
	 * \f$v_i' = v_i + \lambda_1\cdot L(v_i)\f$, where \f$L(v_i)\f$
	 * depends on the type of weight used.
	 * @param l2 Parameter used in moving vertices:
	 * \f$v_i'' = v_i' + \lambda_2\cdot L(v_i')\f$, where \f$L(v_i')\f$
	 * depends on the type of weight used.
	 * @param nit Number of iterations of the algorithm.
	 * @param n_threads Number of threads.
	 * @param m The mesh used to iterate on the neighbourhood of @e old_verts.
	 * @param[in] old_verts Initially, the vertices of the mesh.
	 * @param[out] new_verts At the end of algorithm they contain the final
	 * coordinates of the vertices of the mesh.
	 */
	void apply_twice_per_it
	(
		const smooth_weight& w,
		float l1, float l2,
		size_t nit, size_t n_threads,
		const TriangleMesh& m,
		glm::vec3 *old_verts,
		glm::vec3 *new_verts
	);

} // -- namespace local_private
} // -- namespace smoothing
} // -- namespace geoproc
