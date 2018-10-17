#pragma once

// C++ includes
#include <iostream>
#include <vector>
using namespace std;

// geometry processing algorithms
#include <algorithms/iterators/mesh_iterator.hpp>
#include <algorithms/iterators/vertex_iterators.hpp>
#include <algorithms/triangle_mesh.hpp>
#include <algorithms/ply_reader.hpp>

namespace test_algorithms {

	void test_iterate(int argc, char *argv[]);
	void test_curvature(int argc, char *argv[]);

} // -- namespace test_algorithms
