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
