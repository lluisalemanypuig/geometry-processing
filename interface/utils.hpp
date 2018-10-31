#pragma once

// C++ includes
#include <vector>
#include <chrono>
using namespace std::chrono;

// glm includes
#include <glm/vec3.hpp>

namespace timing {

	typedef high_resolution_clock::time_point time_point;

	// Returns the current time
	time_point now();

	// Returns the elapsed time between 'begin' and 'end' in seconds
	double elapsed_seconds(const time_point& begin, const time_point& end);

	// Returns the elapsed time between 'begin' and 'end' in microseconds
	double elapsed_milliseconds(const time_point& begin, const time_point& end);

	// Returns the elapsed time between 'begin' and 'end' in microseconds
	double elapsed_microseconds(const time_point& begin, const time_point& end);

} // -- namespace timing

namespace coloring {

	/*
	 * Makes colours using the values in data. The colours are
	 * interpolated so that they make the colours of the rainbow.
	 *
	 * The heuristic used is binning.
	 */
	void colors_rainbow_binning
	(const std::vector<float>& data, std::vector<glm::vec3>& cols);


} // -- namespace coloring
