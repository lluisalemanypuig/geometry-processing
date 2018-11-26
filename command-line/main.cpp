// C includes
#include <string.h>

// C++ includes
#include <iostream>
using namespace std;

// Custom includes
#include "test_geoproc.hpp"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		cerr << "Wrong usage: ./command-line algorithm-type" << endl;
		cerr << "    Use [-h|--help] to see the complete list of algorithms available" << endl;
		return 1;
	}

	if (strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
		cout << "List of tests:" << endl;
		cout << "    iterate" << endl;
		cout << "    boundaries" << endl;
		cout << "    curvature" << endl;
		cout << "    local-smoothing" << endl;
		cout << "    global-smoothing" << endl;
		cout << "    band-frequencies" << endl;
		cout << "Use [-h|--help] on every option to see its usage." << endl;
		return 0;
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

	cerr << "Error: algorithm '" << string(argv[1]) << "' not recognised" << endl;
	cerr << "    Use [-h|--help] to see the complete list of algorithms available" << endl;
	return 1;
}
