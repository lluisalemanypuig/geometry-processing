#pragma once

// C++ includes
#include <chrono>
using namespace std::chrono;

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
