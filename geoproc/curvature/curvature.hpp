/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

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
	const TriangleMesh& mesh, std::vector<double>& Kg,
	double *min = nullptr, double *max = nullptr
);

/**
 * @brief Computes the Gauss curvature for each vertex.
 *
 * Use the discretisation of the Laplace-Beltrami operator,
 * using the cotangents as weights.
 *
 * If the number of threads given is 1 then
 * @ref Gauss(const TriangleMesh&, std::vector<double>&,double*,double*)
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
void Gauss(const TriangleMesh& mesh, std::vector<double>& Kg, size_t n_threads);
/**
 * @brief Computes the Gauss curvature for each vertex.
 *
 * Use the discretisation of the Laplace-Beltrami operator,
 * using the cotangents as weights.
 *
 * If the number of threads given is 1 then
 * @ref Gauss(const TriangleMesh&, std::vector<double>&,double*,double*) is called.
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
	const TriangleMesh& mesh, std::vector<double>& Kg,
	size_t n_threads, double *min, double *max
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
	const TriangleMesh& mesh, std::vector<double>& Kh,
	double *min = nullptr, double *max = nullptr
);
/**
 * @brief Computes the mean curvature for each vertex.
 *
 * Use the discretisation of the Laplace-Beltrami operator,
 * using the cotangents as weights.
 *
 * If the number of threads given is 1 then
 * @ref mean(const TriangleMesh&, std::vector<double>&,double*,double*)
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
void mean(const TriangleMesh& mesh, std::vector<double>& Kh, size_t n_threads);
/**
 * @brief Computes the mean curvature for each vertex.
 *
 * Use the discretisation of the Laplace-Beltrami operator,
 * using the cotangents as weights.
 *
 * If the number of threads given is 1 then
 * @ref mean(const TriangleMesh&, std::vector<double>&,double*,double*)
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
	const TriangleMesh& mesh, std::vector<double>& Kh,
	size_t n_threads, double *min, double *max
);

} // -- namespace curavture
} // -- namespace geoproc
