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

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/definitions.hpp>

namespace geoproc {
namespace smoothing {
namespace local {

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
 * where \f$ L(v_i) \f$ is the Laplacian operator defined
 * in @ref modifier::Laplacian.
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
 * @param[out] m Mesh to be smoothed.
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @pre The mesh can't have boundaries if the
 * @ref weight::cotangent is applied. However, this is
 * not checked since the mesh need not have its boundaries computed.
 */
void laplacian
(const weight& w, double lambda, size_t n_iter, TriangleMesh& m);
/**
 * @brief Parallel Laplacian operator for mesh smoothing.
 *
 * See @ref laplacian(const weight&, double, size_t, TriangleMesh&)
 * for details.
 * @param w The type of weight used.
 * @param lambda Smoothing parameter.
 * @param n_iter Number of iterations of the algorithm.
 * @param nt Number of threads.
 * @param[out] m Mesh to be smoothed.
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @pre The mesh can't have boundaries if the
 * @ref weight::cotangent is applied. However, this is
 * not checked since the mesh need not have its boundaries computed.
 */
void laplacian
(const weight& w, double lambda, size_t n_iter, size_t nt, TriangleMesh& m);

/**
 * @brief Bilaplacian operator for mesh smoothing.
 *
 * Applies the Laplacian smothing operator once with positive value of
 * \f$\lambda\f$ and then with negative value of \f$\lambda\f$
 * for every iteration.
 *
 * For details on the Laplacian operator see @ref modifier::Laplacian.
 * For details on how it is applied to smoothing see
 * @ref laplacian(const weight&, double, size_t, TriangleMesh&).
 * @param w The type of weight used.
 * @param lambda Smoothing parameter.
 * @param n_iter Number of iterations of the algorithm.
 * @param[out] m Mesh to be smoothed.
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @pre The mesh can't have boundaries if the
 * @ref weight::cotangent is applied. However, this is
 * not checked since the mesh need not have its boundaries computed.
 */
void bilaplacian
(const weight& w, double lambda, size_t n_iter, TriangleMesh& m);
/**
 * @brief Parallel Bilaplacian operator for mesh smoothing.
 *
 * See @ref bilaplacian(const weight&, double, size_t, TriangleMesh&)
 * for details.
 * @param w The type of weight used.
 * @param lambda Smoothing parameter.
 * @param n_iter Number of iterations of the algorithm.
 * @param nt Number of threads.
 * @param[out] m Mesh to be smoothed.
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @pre The mesh can't have boundaries if the
 * @ref weight::cotangent is applied. However, this is
 * not checked since the mesh need not have its boundaries computed.
 */
void bilaplacian
(const weight& w, double lambda, size_t n_iter, size_t nt, TriangleMesh& m);

/**
 * @brief Taubin \f$\lambda-\mu\f$ operator for mesh smoothing.
 *
 * Applies the Laplacian operator once with \f$\lambda\f$ and
 * then with \f$\mu\f$ for every iteration.
 *
 * The value \f$\mu\f$ is determined isolating it from the
 * following formula:
 *
 * \f$K_{PB} = \frac{1}{\lambda} + \frac{1}{\mu} \Rightarrow
 * \mu = \left( K_{PB} - \frac{1}{\lambda} \right)^{-1}\f$
 *
 * The value of \f$K_{PB}\f$ has been set to \f$K_{PB}=0.1\f$.
 *
 * @param w The type of weight used.
 * @param lambda Smoothing parameter.
 * @param n_iter Number of iterations of the algorithm.
 * @param[out] m Mesh to be smoothed.
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @pre The mesh can't have boundaries if the
 * @ref weight::cotangent is applied. However, this is
 * not checked since the mesh need not have its boundaries computed.
 */
void TaubinLM
(const weight& w, double lambda, size_t n_iter, TriangleMesh& m);
/**
 * @brief Parallel Taubin \f$\lambda-\mu\f$ operator for mesh smoothing.
 *
 * See @ref TaubinLM(const weight&, double, size_t, TriangleMesh&)
 * for details.
 * @param w The type of weight used.
 * @param lambda Smoothing parameter.
 * @param n_iter Number of iterations of the algorithm.
 * @param nt Number of threads.
 * @param[out] m Mesh to be smoothed.
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @pre The mesh can't have boundaries if the
 * @ref weight::cotangent is applied. However, this is
 * not checked since the mesh need not have its boundaries computed.
 */
void TaubinLM
(const weight& w, double lambda, size_t n_iter, size_t nt, TriangleMesh& m);

} // -- namespace local
} // -- namespace smoothing
} // -- namespace geoproc
