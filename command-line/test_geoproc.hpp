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

// Custom includes
#include "time.hpp"

namespace test_geoproc {

	int test_iterate(int argc, char *argv[]);
	int test_curvature(int argc, char *argv[]);
	int test_local_smoothing(int argc, char *argv[]);
	int test_high_frequency(int argc, char *argv[]);

} // -- namespace test_algorithms
