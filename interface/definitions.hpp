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

// glm includes
#include <glm/detail/type_vec.hpp>
#include <glm/detail/qualifier.hpp>

// custom extension of namespace glm
namespace glm {

typedef glm::vec<2, float, glm::qualifier::highp> vec2f;
typedef glm::vec<2, double, glm::qualifier::highp> vec2d;

typedef glm::vec<3, float, glm::qualifier::highp> vec3f;
typedef glm::vec<3, double, glm::qualifier::highp> vec3d;
typedef glm::vec<3, int, glm::qualifier::highp> vec3i;

} // -- namespace glm
