#pragma once

// C includes
#include <string.h>

// C++ includes
#include <iostream>
#include <vector>
using namespace std;

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>

#include <geoproc/curvature/curvature.hpp>
#include <geoproc/smoothing/iterative_local.hpp>

#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>

// Custom includes
#include "time.hpp"

namespace test_algorithms {

	int test_iterate(int argc, char *argv[]);
	int test_curvature(int argc, char *argv[]);
	int test_smoothing(int argc, char *argv[]);

} // -- namespace test_algorithms
