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
		const vector<float>& data,
		float dmin, float step,
		float val, bool around_val,
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
		size_t ndata, float prop,
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
			(100.0f*count)/ndata < prop and
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
		const vector<float>& data, float val, bool around_val,
		float dmin, float dmax, float prop,
		float& m, float& M
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
		if (dmax - dmin <= 100.0f)			{ nbins = 100; }
		else if (dmax - dmin <= 1000.0f)	{ nbins = 100; }
		else if (dmax - dmin <= 10000.0f)	{ nbins = 1000; }
		else if (dmax - dmin <= 100000.0f)	{ nbins = 10000; }
		else if (dmax - dmin <= 1000000.0f)	{ nbins = 100000; }
		else								{ nbins = 1000000; }
		++nbins;
		int inbins = static_cast<int>(nbins);

		float step = (dmax - dmin)/(nbins - 1);

		#if defined (DEBUG)
		cout << "    use " << nbins << " bins" << endl;
		cout << "    step: " << step << endl;
		#endif

		// allocate and initialise memory for bins
		int *bins = static_cast<int *>(malloc((nbins + 1)*sizeof(int)));

		int left, right;

		int count;
		size_t num_steps = 0;
		float current_prop, err;
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

			current_prop = (100.0f*count)/ndata;
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
			if (err < 0.001f and current_prop > prop) {
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
		const vector<float>& data,
		float dmin, float dmax, float prop,
		float& min, float& max
	)
	{
		generic_binning(data, 0.0, false, dmin, dmax, prop, min, max);
	}

	void binning_around(
		const vector<float>& data, float center,
		float dmin, float dmax, float prop,
		float& min, float& max
	)
	{
		generic_binning(data, center, true, dmin, dmax, prop, min, max);
	}

	void below_dev(
		const vector<float>& data,
		float prop, float& min, float& max
	)
	{
		float mean = 0.0f;
		for (float v : data) {
			mean += v;
		}
		mean /= data.size();
		float var = 0.0f;
		for (float v : data) {
			var += (v - mean)*(v - mean);
		}
		var /= (data.size() - 1);

		float vm = numeric_limits<float>::max();
		float vM = -numeric_limits<float>::max();
		float dev = sqrt(var);
		dev *= (1.0f + prop/100.0f);

		for (float v : data) {
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
		const vector<float>& values, float m, float M,
		vector<glm::vec3>& cols
	)
	{
		// ----------------------- //
		// Colour rainbow gradient //
		float r, g, b;
		r = g = b = 0.0f;
		cols = vector<glm::vec3>(values.size());
		for (size_t i = 0; i < values.size(); ++i) {
			float v = values[i];
			float s = (v - m)/(M - m);

			if (s <= 0.0f) {
				// RED
				// below 0.0
				r = 1.0f;
				g = 0.0f;
				b = 0.0f;
			}
			else if (s <= 0.2f) {
				// RED to YELLOW
				// from 0.0 to 0.2
				r = 1.0f;
				g = 5.0f*(s - 0.0f);
				b = 0.0f;
			}
			else if (s <= 0.4f) {
				// YELLOW to GREEN
				// from 0.2 to 0.4
				r = -5.0f*s + 2.0f;
				g = 1.0f;
				b = 0.0f;
			}
			else if (s <= 0.6f) {
				// GREEN to CYAN
				// from 0.4 to 0.6
				r = 0.0f;
				g = 1.0f;
				b = 5.0f*(s - 0.4f);
			}
			else if (s <= 0.8f) {
				// CYAN to BLUE
				// from 0.6 to 0.8
				r = 0.0f;
				g = -5.0f*s + 4.0f;
				b = 1.0f;
			}
			else if (s <= 1.0f) {
				// BLUE to PURPLE
				// from 0.8 to 1.0
				r = 5.0f*(s - 0.8f);
				g = 0.0f;
				b = 1.0f;
			}
			else if (1.0f <= s) {
				// PURPLE
				// above 1.0
				r = 1.0f;
				g = 0.0f;
				b = 1.0f;
			}

			cols[i].x = r;
			cols[i].y = g;
			cols[i].z = b;
		}
	}

} // -- namespace colors
