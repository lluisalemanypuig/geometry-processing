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

namespace coloring {

	void colors_rainbow_binning
	(const std::vector<float>& values, std::vector<glm::vec3>& cols)
	{
		ofstream fout;

		fout.open("curvatures");
		fout << "i\tc" << endl;
		// find minimum and maximum values of curvature
		float vm = numeric_limits<float>::max();
		float vM = -numeric_limits<float>::max();
		for (size_t i = 0; i < values.size(); ++i) {
			vm = std::min(vm, values[i]);
			vM = std::max(vM, values[i]);
			fout << i << "\t" << values[i] << endl;
		}
		fout.close();

		cout << "Binning to make rainbow:" << endl;
		cout << "    total values: " << cols.size() << endl;
		cout << "    min value: " << vm << endl;
		cout << "    max value: " << vM << endl;
		cout << "        difference: " << vM - vm << endl;

		// do binning on # of bins as a function
		// of the difference between the maximum
		// and minimum values

		int nbins;
		if (vM - vm <= 10.0f)			{ nbins = 1; }
		else if (vM - vm <= 100.0f)		{ nbins = 10; }
		else if (vM - vm <= 1000.0f)	{ nbins = 100; }
		else if (vM - vm <= 10000.0f)	{ nbins = 1000; }
		else if (vM - vm <= 100000.0f)	{ nbins = 10000; }
		else if (vM - vm <= 1000000.0f)	{ nbins = 100000; }
		else							{ nbins = 1000000; }
		float step = (vM - vm)/nbins;

		cout << "    use " << nbins << " bins" << endl;
		cout << "    step= " << step << endl;

		vector<int> bins(nbins + 1, 0);

		// i-th bin comprises the values
		//
		//  [ m + i*(M - m)/nbins,
		//    m + (i + 1)*(M - m)/nbins] =
		// = [m + i*step, m + (i + 1)*step]
		//

		// maximum size among the bins,
		// and the corresponding index
		int max_size = 0;
		int max_idx = 0;
		for (size_t i = 0; i < values.size(); ++i) {
			int idx = (values[i] - vm)/step;
			++bins[idx];

			if (max_size < bins[idx]) {
				max_size = bins[idx];
				max_idx = idx;
			}
		}

		fout.open("bins");
		fout << "x\ty" << endl;
		for (size_t i = 0; i < bins.size(); ++i) {
			fout << i << "\t" << bins[i] << endl;
		}
		fout.close();

		cout << "    found largest bin at: " << max_idx << endl;
		cout << "        with " << bins[max_idx] << " values in it" << endl;

		// amount of curvature values covered
		int count = max_size;
		int left = max_idx;
		int right = max_idx;

		do {
			if (left > 0 and bins[left] > bins[right]) {
				--left;
				count += bins[left];
			}
			else if (right < nbins) {
				++right;
				count += bins[right];
			}
		}
		while (
			(100.0f*count)/values.size() < 80.0f and
			(left > 0 or right < nbins)
		);

		cout << "    Found 80% of values between: " << endl;
		cout << "        left=  " << left << endl;
		cout << "        right= " << right << endl;
		cout << "        count= " << count << endl;
		cout << "            proportion: " << float(count)/values.size() << endl;

		// compute actual minimum and maximum
		vM = vm + right*step;
		vm = vm + left*step;

		cout << "    new minimum: " << vm << endl;
		cout << "    new maximum: " << vM << endl;

		// ----------------------- //
		// Colour rainbow gradient //
		cols = vector<glm::vec3>(values.size());
		for (size_t i = 0; i < values.size(); ++i) {
			float v = values[i];
			float s = (v - vm)/(vM - vm);

			float r, g, b;
			if (s <= 0.0f) {
				r = 0.0f;
				g = 0.0f;
				b = 0.0f;
			}
			else if (s <= 0.2f) {
				// RED
				// from 0.0 to 0.2
				r = 5.0f*(s - 0.0f);
				g = 0.0f;
				b = 0.0f;
			}
			else if (s <= 0.4f) {
				// YELLOW
				// from 0.2 to 0.4
				r = 1.0f;
				g = 5.0f*(s - 0.2f);
				b = 0.0f;
			}
			else if (s <= 0.6f) {
				// GREEN
				// from 0.4 to 0.6
				r = 5.0f*(s - 0.4f);
				g = 1.0f;
				b = 0.0f;
			}
			else if (s <= 0.8f) {
				// TURQUOISE
				// from 0.6 to 0.8
				r = 0.0f;
				g = 1.0f;
				b = 5.0f*(s - 0.6f);
			}
			else if (s <= 1.0f) {
				// BLUE
				// from 0.8 to 1.0
				r = 0.0f;
				g = 5.0f*(s - 0.8f);
				b = 1.0f;
			}
			else if (1.0f <= s) {
				r = 5.0f*(s - 1.0f);
				g = 1.0f;
				b = 1.0f;
			}

			cols[i].x = r;
			cols[i].y = g;
			cols[i].z = b;
		}
	}

} // -- namespace colors
