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

namespace min_max {

	/* Finds the minimum and maximum values within the set of values
	 * 'data' that comprise most of the values of data.
	 *
	 * This "most" is guided by the tolerant (%).
	 * For example:
	 * - if prop = 100.0 then min and max will take the
	 * minimum and maximum values in data.
	 * - if prop = 50.0 then min and max will take two values
	 * in data so that 50% of the values in data are between min and max,
	 * both included.
	 *
	 * The algorithm starts finding the most occupied bin and computes
	 * a range containing this maximum in which we find the (%) of
	 * values.
	 */
	void binning(
		const std::vector<float>& data,
		float dmin, float dmax, float prop,
		float& min, float& max
	);

	/* Finds the minimum and maximum values within the set of values
	 * 'data' that comprise most of the values of data starting at
	 * a given value.
	 *
	 * This "most" is guided by the tolerant (%).
	 * For example:
	 * - if prop = 100.0 then min and max will take the
	 * minimum and maximum values in data.
	 * - if prop = 50.0 then min and max will take two values
	 * in data so that 50% of the values in data are between min and max,
	 * both included.
	 *
	 * The algorithm starts at the bin corresponding to the given value
	 * (center) and computes a range containing this maximum in which we find
	 * the (%) of values.
	 */
	void binning_around(
		const std::vector<float>& data, float center,
		float dmin, float dmax, float prop,
		float& min, float& max
	);

	/* Find the minimum and maximum value among those
	 * that are below the quantity
	 *
	 * (1 + prop)*dev
	 *
	 * where dev is the square root of the variance of
	 * the curvature values.
	 */
	void below_dev(
		const std::vector<float>& data,
		float prop, float& min, float& max
	);

} // -- namespace min_max

namespace coloring {

	/* Makes colours using the values in data. The colours are
	 * interpolated so that they make the colours of the rainbow.
	 *
	 * The heuristic used is binning.
	 */
	void colors_rainbow(
		const std::vector<float>& data, float m, float M,
		std::vector<glm::vec3>& cols
	);

} // -- namespace coloring
