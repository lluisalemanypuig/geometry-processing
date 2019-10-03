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

// glm includes
#include <glm/vec3.hpp>

// geoproc includes
#include <geoproc/definitions.hpp>
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace parametrisation {

/**
 * @brief Calculates the texture coordinates using Harmonic Maps.
 *
 * These coordinates are obtained by solving the linear equation
 * system: \f$Ax = b\f$ whose construction is similar to that in
 * global smoothing (see @ref smoothing::global::smooth).
 *
 * Matrix @e A is defined as
 * - \f$A_{ii} = -1\f$
 * - \f$A_{ij} = w_{ij}, \forall i \neq j\f$
 * where \f$ 0 \le i,j \le N\f$.
 *
 * The values \f$w_{ij}\f$ are the weights for vertex \f$v_i\f$, and their
 * values depend on parameter @e w. Matrix \f$A\f$ has dimensions
 * \f$N \times N\f$, where \f$N\f$ is the number of vertices of the mesh.
 * (keep reading to know how fixed vertices are considered...).
 *
 * The new coordinates of the vertices of the mesh is the solution \f$x\f$
 * of the equation:
 *
 * \f$Ax_x = b_x\f$, \f$Ax_y = b_y\f$
 *
 * where \f$b_x,b_y\f$ contain some values. When the mesh has one single
 * boundary, its vertices are considered fixed.
 *
 * The vertices defined as constant are moved to the right hand-side of the
 * equation and the matrix of the system becomes \f$A'\f$ and has dimensions
 * \f$v \times v\f$ where \f$v\f$ is the number of variable vertices:
 *
 * \f$v = N - v^*\f$, where \f$v^*\f$ is the amount of fixed vertices.
 *
 * The systems are solved using the Simplicial Cholesky method from library
 * Eigen in a least square sense:
 *
 * \f$ A^TAx_x = A^Tb_x\f$, \f$A^TAx_y = A^Tb_y\f$
 *
 * In order to save time, the decomposition needed for the Cholesky
 * method is computed only once and reused to solve the three systems
 * defined above.
 *
 * Now, the Laplacian operator defines the linear equation for vertex
 * \f$v_i\f$ as:
 *
 * \f$L(v_i) = \left(\sum_{j \in N(i)} w_{ij}*v_j\right) - v_i\f$
 *
 * @param m Input mesh. It must have one single boundary.
 * @param w Type of weights to use to compute the parametrisation.
 * @param s Shape of the boundary vertices on the texture.
 * @param[out] uvs Texture coordinates of the vertices of @e m in space
 * [0,0] x [1,1].
 * @pre The mesh requires:
 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
 * - Boundaries (see @ref TriangleMesh::make_boundaries)
 * - Angles and areas (see @ref TriangleMesh::make_angles_area)
 * @return Returns true on success. The procedure might fail if there
 * are no boundaries on the mesh or there are more than one.
 */
bool harmonic_maps
(const TriangleMesh& m, const weight& w,
 const boundary_shape& s, std::vector<glm::vec2d>& uvs);

} // -- namespace parametrisation
} // -- namespace geoproc
