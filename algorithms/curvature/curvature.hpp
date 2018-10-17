#pragma once

// C includes
#include <omp.h>

// C++ includes
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// Custom includes
#include <algorithms/triangle_mesh.hpp>
#include <algorithms/iterators/mesh_iterator.hpp>
#include <algorithms/iterators/vertex_iterators.hpp>

namespace algorithms {
namespace curvature {

	/**
	 * @brief Computes the Gauss curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 * @param mesh Input mesh.
	 * @param[out] Kg An approximation of the Gauss curvature per vertex.
	 * @param n_threads Number of threads.
	 */
	void Gauss(const TriangleMesh& mesh, vector<float>& Kg, size_t n_threads = 1);

	/**
	 * @brief Computes the mean curvature for each vertex.
	 *
	 * Use the discretisation of the Laplace-Beltrami operator,
	 * using the cotangents as weights.
	 * @param mesh Input mesh.
	 * @param[out] Kh An approximation of the mean curvature per vertex.
	 * @param n_threads Number of threads.
	 */
	void mean(const TriangleMesh& mesh, vector<float>& Kh, size_t n_threads = 1);

} // -- namespace curavture
} // -- namespace algorithms
