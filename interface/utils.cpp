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

	void generic_binning(
		const std::vector<float>& data, float val, bool around_val,
		float& min, float& max, float prop
	)
	{
		// find minimum and maximum values of curvature
		float vm = numeric_limits<float>::max();
		float vM = -numeric_limits<float>::max();
		for (size_t i = 0; i < data.size(); ++i) {
			vm = std::min(vm, data[i]);
			vM = std::max(vM, data[i]);
		}

		#if defined (DEBUG)
		cout << "Binning start up:" << endl;
		cout << "    total values: " << data.size() << endl;
		cout << "    min value: " << vm << endl;
		cout << "    max value: " << vM << endl;
		cout << "        difference: " << vM - vm << endl;
		#endif

		// do binning on # of bins as a function
		// of the difference between the maximum
		// and minimum values

		size_t nbins;
		if (vM - vm <= 10.0f)			{ nbins = 1; }
		else if (vM - vm <= 100.0f)		{ nbins = 10; }
		else if (vM - vm <= 1000.0f)	{ nbins = 100; }
		else if (vM - vm <= 10000.0f)	{ nbins = 1000; }
		else if (vM - vm <= 100000.0f)	{ nbins = 10000; }
		else if (vM - vm <= 1000000.0f)	{ nbins = 100000; }
		else							{ nbins = 1000000; }
		float step = (vM - vm)/nbins;

		#if defined (DEBUG)
		cout << "    use " << nbins << " bins" << endl;
		cout << "    step= " << step << endl;
		#endif

		int *bins = static_cast<int *>(malloc((nbins + 1)*sizeof(int)));
		for (size_t i = 0; i < nbins + 1; ++i) {
			bins[i] = 0;
		}

		// i-th bin comprises the values
		//
		//  [ m + i*(M - m)/nbins,
		//    m + (i + 1)*(M - m)/nbins] =
		// = [m + i*step, m + (i + 1)*step]
		//

		// maximum size among the bins,
		// and the corresponding index
		int center_size = 0;
		int center_idx = 0;
		for (size_t i = 0; i < data.size(); ++i) {
			int idx = (data[i] - vm)/step;
			++bins[idx];

			if (center_size < bins[idx]) {
				center_size = bins[idx];
				center_idx = idx;
			}
		}

		if (around_val) {
			int idx = int((val - vm)/step);
			center_idx = idx;
			center_size = bins[idx];
		}

		#if defined (DEBUG)
		cout << "Binning result:" << endl;
		cout << "    starting bin at: " << center_idx << endl;
		cout << "        with " << bins[center_idx] << " values in it" << endl;
		#endif

		// amount of curvature values covered
		int count = center_size;
		int left = center_idx;
		int right = center_idx;
		bool move_left = (0 < left ? true : false);
		bool move_right = (right < nbins ? true : false);

		while (
			(100.0f*count)/data.size() < prop and
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

		#if defined (DEBUG)
		cout << "    Found " << prop << "% of values between: " << endl;
		cout << "        left=  " << left << endl;
		cout << "        right= " << right << endl;
		cout << "        count= " << count << endl;
		cout << "            proportion: " << float(count)/data.size() << endl;
		#endif

		// compute actual minimum and maximum
		vM = vm + right*step;
		vm = vm + left*step;

		#if defined (DEBUG)
		cout << "    new minimum: " << vm << endl;
		cout << "    new maximum: " << vM << endl;
		#endif

		min = vm;
		max = vM;

		free(bins);
	}

	void binning(
		const std::vector<float>& data,
		float& min, float& max, float prop
	)
	{
		generic_binning(data, 0.0, false, min, max, prop);
	}

	void binning_around(
		const std::vector<float>& data, float center,
		float& min, float& max, float prop
	)
	{
		generic_binning(data, center, true, min, max, prop);
	}

	void below_dev(
		const std::vector<float>& data,
		float& min, float& max, float prop
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
		float dev = std::sqrt(var);
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
		const std::vector<float>& values, float m, float M,
		std::vector<glm::vec3>& cols
	)
	{
		// ----------------------- //
		// Colour rainbow gradient //
		cols = vector<glm::vec3>(values.size());
		for (size_t i = 0; i < values.size(); ++i) {
			float v = values[i];
			float s = (v - m)/(M - m);

			float r, g, b;
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
