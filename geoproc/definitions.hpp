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
#include <cstdint>

// glm includes
#include <glm/detail/type_vec.hpp>
#include <glm/detail/qualifier.hpp>

// custom extension of namespace glm
namespace glm {

typedef vec<2, float, glm::qualifier::highp> vec2f;
typedef vec<2, double, glm::qualifier::highp> vec2d;

typedef vec<3, float, glm::qualifier::highp> vec3f;
typedef vec<3, double, glm::qualifier::highp> vec3d;
typedef vec<3, int, glm::qualifier::highp> vec3i;

} // -- namespace glm

namespace geoproc {

/**
 * @brief The different types of modifiers available.
 *
 * - Laplacian: see @ref smooth_modifier::Laplacian.
 * - Bi-Laplacian: see @ref smooth_modifier::BiLaplacian.
 * - TaubinLM: see @ref smooth_modifier::TaubinLM.
 *
 * Since 'operator' is a C++'s keyword, the word 'modifier' is
 * used instead.
 */
enum class modifier : int8_t {
	/**
	 * The Laplacian operator on a vertex is formally defined as:
	 * \f$ L(v_i) = \sum_{j} w_{ij}(v_j - v_i) \f$
	 *
	 * where \f$w_{ij}\f$ is the type of weight. See @ref weight for details.
	 *
	 * Its usage is detailed in functions:
	 * - @ref smoothing::local::laplacian(const weight&, double, size_t, TriangleMesh&).
	 * - @ref smoothing::local::laplacian(const weight&, double, size_t, size_t, TriangleMesh&).
	 */
	Laplacian,
	/**
	 * The Bilaplacian operator applies twice the @ref modifier::Laplacian operator.
	 *
	 * Its usage is detailed in functions:
	 * - @ref smoothing::local::bilaplacian(const weight&, double, size_t, TriangleMesh&)
	 * - @ref smoothing::local::bilaplacian(const weight&, double, size_t, size_t, TriangleMesh&)
	 */
	BiLaplacian,
	/**
	 * The TaubinLM operator applies twice the @ref modifier::Laplacian operator.
	 *
	 * Detailed in the documentation of functions:
	 * - @ref smoothing::local::TaubinLM(const weight&, double, size_t, TriangleMesh&)
	 * - @ref smoothing::local::TaubinLM(const weight&, double, size_t, size_t, TriangleMesh&)
	 */
	TaubinLM
};

/**
 * @brief The different types of weights.
 *
 * - uniform: see @ref weight::uniform.
 * - cotangent: see @ref weight::cotangent.
 */
enum class weight : int8_t {
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
	 * \f$ \omega_{ij} = \cot(\alpha) + \cot(\beta) \f$
	 */
	cotangent
};

/**
 * @brief Shapes for boundary vertices.
 *
 * Sometimes we may want to place the vertices at the boundary of a mesh
 * in a certain shape on the xy plane. The shapes are the following:
 * - circle: see @ref boundary_shape::Circle.
 * - square: see @ref boundary_shape::Square.
 */
enum class boundary_shape : int8_t {
	/// The boundary vertices are placed on the circumference of
	/// a circle of radius 1, centered at (0,0).
	Circle = 0,
	/// The boundary vertices are placed on the boundary of
	/// a square of side length 2, centered at (0,0).
	Square
};

} // -- namespace geoproc
