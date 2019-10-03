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

#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

namespace timing {

time_point now() {
	return high_resolution_clock::now();
}

double elapsed_seconds(const time_point& begin, const time_point& end) {
	return duration<double, seconds::period>( end - begin ).count();
}

double elapsed_milliseconds(const time_point& begin, const time_point& end) {
	return duration<double, milliseconds::period>( end - begin ).count();
}

double elapsed_microseconds(const time_point& begin, const time_point& end) {
	return duration<double, microseconds::period>( end - begin ).count();
}

} // -- namespace timing

namespace min_max {

static inline
void init_bins(size_t nbins, int *bins) {
	for (size_t i = 0; i < nbins + 1; ++i) {
		bins[i] = 0;
	}
}

static inline
void fill_bins
(
	const vector<double>& data,
	double dmin, double step, double val, bool around_val,
	int nbins, int *bins, int& start_at
)
{
	/* fill bins */

	// maximum size among the bins,
	// and the corresponding index
	int max_size = 0;
	start_at = 0;
	for (size_t i = 0; i < data.size(); ++i) {
		int idx = int((data[i] - dmin)/step);

		// since we are doing adaptative binning
		// the index may be out of bounds
		if (idx < 0 or idx > nbins) {
			continue;
		}

		++bins[idx];
		if (max_size < bins[idx]) {
			max_size = bins[idx];
			start_at = idx;
		}
	}

	if (around_val) {
		int idx = int((val - dmin)/step);
		start_at = idx;
		if (start_at < 0) {
			start_at = 0;
		}
		else if (start_at > nbins) {
			start_at = nbins;
		}
	}
}

static inline
int find_left_right
(
	const int *bins, int nbins,
	size_t ndata, double prop,
	int start_at, int& left, int& right
)
{
	// amount of curvature values covered
	int count = bins[start_at];
	left = start_at;
	right = start_at;
	bool move_left = (0 < left ? true : false);
	bool move_right = (right < nbins ? true : false);

	while (
		(100.0*count)/ndata < prop and
		(move_left or move_right)
	)
	{
		if (move_left and move_right) {
			if (bins[left - 1] > bins[right + 1]) {
				count += bins[left - 1];
				--left;
				move_left = (0 < left ? true : false);
			}
			else {
				count += bins[right + 1];
				++right;
				move_right = (right < nbins ? true : false);
			}
		}
		else {
			// can move only one
			if (move_left) {
				count += bins[left - 1];
				--left;
				move_left = (0 < left ? true : false);
			}
			else {
				count += bins[right + 1];
				++right;
				move_right = (right < nbins ? true : false);
			}
		}
	}
	return count;
}

void generic_binning(
	const vector<double>& data, double val, bool around_val,
	double dmin, double dmax, double prop,
	double& m, double& M
)
{
	size_t ndata = data.size();

	#if defined (DEBUG)
	cout << "-------------------------" << endl;
	cout << "Binning start up:" << endl;
	cout << "    total values: " << ndata << endl;
	cout << "    min value: " << dmin << endl;
	cout << "    max value: " << dmax << endl;
	cout << "        difference: " << dmax - dmin << endl;
	#endif

	// do binning on # of bins as a function
	// of the difference between the maximum
	// and minimum values

	size_t nbins;
	if (dmax - dmin <= 100.0)			{ nbins = 100; }
	else if (dmax - dmin <= 1000.0)		{ nbins = 100; }
	else if (dmax - dmin <= 10000.0)	{ nbins = 1000; }
	else if (dmax - dmin <= 100000.0)	{ nbins = 10000; }
	else if (dmax - dmin <= 1000000.0)	{ nbins = 100000; }
	else								{ nbins = 1000000; }
	++nbins;
	int inbins = static_cast<int>(nbins);

	double step = (dmax - dmin)/(nbins - 1);

	#if defined (DEBUG)
	cout << "    use " << nbins << " bins" << endl;
	cout << "    step: " << step << endl;
	#endif

	// allocate and initialise memory for bins
	int *bins = static_cast<int *>(malloc((nbins + 1)*sizeof(int)));

	int left, right;

	int count;
	size_t num_steps = 0;
	double current_prop, err;
	bool terminate = false;
	do {
		init_bins(nbins, bins);

		// dump data into the bins
		int start_at;
		fill_bins(data, dmin, step, val, around_val, inbins, bins, start_at);

		// find the left and right bins so that the collection
		// of values between them
		count = find_left_right
			(bins, inbins, ndata, prop, start_at, left, right);

		current_prop = (100.0*count)/ndata;
		err = std::abs(prop - current_prop);

		// Find values of data in the left and right bins.
		// These are the new "minimum" and "maximum" of the data.
		dmax = dmin + right*step;
		dmin = dmin + left*step;
		// make new step
		step = (dmax - dmin)/(nbins - 1);

		++num_steps;
		if (num_steps >= 100) {
			terminate = true;
		}
		if (err < 0.001 and current_prop > prop) {
			terminate = true;
		}
	}
	while (not terminate);

	// compute current minimum and maximum
	m = dmin;
	M = dmax;

	#if defined (DEBUG)
	cout << "    Binning result:" << endl;
	cout << "        Found " << prop << "% of values between: " << endl;
	cout << "            left=  " << left << endl;
	cout << "            right= " << right << endl;
	cout << "            count= " << count << endl;
	cout << "               actual proportion: " << current_prop << endl;
	cout << "                error: " << err << endl;
	cout << "        new minimum: " << m << endl;
	cout << "        new maximum: " << M << endl;
	cout << "        in " << num_steps + 1 << " steps" << endl;
	#endif

	// free memory allocated for the bins
	free(bins);
}

void binning(
	const vector<double>& data,
	double dmin, double dmax, double prop,
	double& min, double& max
)
{
	generic_binning(data, 0.0, false, dmin, dmax, prop, min, max);
}

void binning_around(
	const vector<double>& data, double center,
	double dmin, double dmax, double prop,
	double& min, double& max
)
{
	generic_binning(data, center, true, dmin, dmax, prop, min, max);
}

void below_dev(
	const vector<double>& data,
	double prop, double& min, double& max
)
{
	double mean = 0.0;
	for (double v : data) {
		mean += v;
	}
	mean /= data.size();
	double var = 0.0;
	for (double v : data) {
		var += (v - mean)*(v - mean);
	}
	var /= (data.size() - 1);

	double vm = numeric_limits<double>::max();
	double vM = -numeric_limits<double>::max();
	double dev = sqrt(var);
	dev *= (1.0 + prop/100.0);

	for (double v : data) {
		if (v < dev) {
			if (v > vM) {
				vM = v;
			}
			if (v < vm) {
				vm = v;
			}
		}
	}
	min = vm;
	max = vM;
}

} // -- namespace min_max

namespace coloring {

void colors_rainbow
(
	const vector<double>& values, double m, double M,
	vector<glm::vec3d>& cols
)
{
	// ----------------------- //
	// Colour rainbow gradient //
	double r, g, b;
	r = g = b = 0.0;
	cols = vector<glm::vec3d>(values.size());
	for (size_t i = 0; i < values.size(); ++i) {
		double v = values[i];
		double s = (v - m)/(M - m);

		if (s <= 0.0) {
			// RED
			// below 0.0
			r = 1.0;
			g = 0.0;
			b = 0.0;
		}
		else if (s <= 0.2) {
			// RED to YELLOW
			// from 0.0 to 0.2
			r = 1.0;
			g = 5.0*(s - 0.0);
			b = 0.0;
		}
		else if (s <= 0.4) {
			// YELLOW to GREEN
			// from 0.2 to 0.4
			r = -5.0*s + 2.0;
			g = 1.0;
			b = 0.0;
		}
		else if (s <= 0.6f) {
			// GREEN to CYAN
			// from 0.4 to 0.6
			r = 0.0;
			g = 1.0;
			b = 5.0*(s - 0.4);
		}
		else if (s <= 0.8f) {
			// CYAN to BLUE
			// from 0.6 to 0.8
			r = 0.0;
			g = -5.0*s + 4.0;
			b = 1.0;
		}
		else if (s <= 1.0) {
			// BLUE to PURPLE
			// from 0.8 to 1.0
			r = 5.0*(s - 0.8);
			g = 0.0;
			b = 1.0;
		}
		else if (1.0 <= s) {
			// PURPLE
			// above 1.0
			r = 1.0;
			g = 0.0;
			b = 1.0;
		}

		cols[i].x = r;
		cols[i].y = g;
		cols[i].z = b;
	}
}

} // -- namespace colors
