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

// C includes
#include <string.h>

// C++ includes
#include <iostream>
using namespace std;

namespace test_geoproc {

	int test_inspect(int argc, char *argv[]);
	int test_iterate(int argc, char *argv[]);
	int test_boundaries(int argc, char *argv[]);
	int test_curvature(int argc, char *argv[]);
	int test_smoothing_local(int argc, char *argv[]);
	int test_smoothing_global(int argc, char *argv[]);
	int test_band_frequency(int argc, char *argv[]);
	int test_harmonic_maps(int argc, char *argv[]);
	int test_remeshing_harmonic_maps(int argc, char *argv[]);

} // -- namespace test_algorithms

void command_line_usage() {
	cout << "List of tests:" << endl;
	cout << "    inspect" << endl;
	cout << "    iterate" << endl;
	cout << "    boundaries" << endl;
	cout << "    curvature" << endl;
	cout << "    local-smoothing" << endl;
	cout << "    global-smoothing" << endl;
	cout << "    band-frequencies" << endl;
	cout << "    harmonic-maps" << endl;
	cout << "    remeshing" << endl;
	cout << "Use [-h|--help] on every option to see its usage." << endl;
}

int main(int argc, char *argv[]) {
	cout << "Geometry Processing Project  Copyright (C) 2018-2019 Lluís Alemany Puig" << endl;
	cout << "This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'." << endl;
	cout << "This is free software, and you are welcome to redistribute it" << endl;
	cout << "under certain conditions; type `show c' for details." << endl;

	if (argc == 1) {
		command_line_usage();
		return 1;
	}

	if (strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
		command_line_usage();
		return 0;
	}

	if (strcmp(argv[1], "inspect") == 0) {
		return test_geoproc::test_inspect(argc, argv);
	}

	if (strcmp(argv[1], "iterate") == 0) {
		return test_geoproc::test_iterate(argc, argv);
	}

	if (strcmp(argv[1], "boundaries") == 0) {
		return test_geoproc::test_boundaries(argc, argv);
	}

	if (strcmp(argv[1], "curvature") == 0) {
		return test_geoproc::test_curvature(argc, argv);
	}

	if (strcmp(argv[1], "local-smoothing") == 0) {
		return test_geoproc::test_smoothing_local(argc, argv);
	}

	if (strcmp(argv[1], "global-smoothing") == 0) {
		return test_geoproc::test_smoothing_global(argc, argv);
	}

	if (strcmp(argv[1], "band-frequencies") == 0) {
		return test_geoproc::test_band_frequency(argc, argv);
	}

	if (strcmp(argv[1], "harmonic-maps") == 0) {
		return test_geoproc::test_harmonic_maps(argc, argv);
	}

	if (strcmp(argv[1], "remeshing") == 0) {
		return test_geoproc::test_remeshing_harmonic_maps(argc, argv);
	}

	cerr << "Error: algorithm '" << string(argv[1]) << "' not recognised" << endl;
	cerr << "    Use [-h|--help] to see the complete list of algorithms available" << endl;
	return 1;
}
