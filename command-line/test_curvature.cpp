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

// geoproc includes
#include <geoproc/curvature/curvature.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

// custom includes
#include "time.hpp"

namespace test_geoproc {

	void curvature_usage() {
		cout << "Curvature evaluation" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --curv K: specify the type of curvature to be evaluated" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        Gauss : to evaluate the Gauss curvature Kg" << endl;
		cout << "        mean  : to evaluate the mean curvature Kh" << endl;
		cout << endl;
		cout << "    --print : print the values of curvature" << endl;
		cout << "        Default: No" << endl;
		cout << endl;
		cout << "    --threads n: specify number of threads." << endl;
		cout << "        Default: 1" << endl;
		cout << endl;
	}

	int test_curvature(int argc, char *argv[]) {
		string mesh_file = "none";
		string curvature = "none";
		size_t nt = 1;
		bool print_curv = false;

		if (argc == 2) {
			curvature_usage();
			return 1;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				curvature_usage();
				return 0;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--curv") == 0) {
				curvature = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--print") == 0) {
				print_curv = true;
			}
			else if (strcmp(argv[i], "--threads") == 0) {
				nt = atoi(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line curvature --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (curvature == "none") {
			cerr << "Error: curvature to be evaluated not specified" << endl;
			cerr << "    Use ./command-line curvature --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		if (curvature != "Gauss" and curvature != "mean") {
			cerr << "Error: curvature value '" << curvature << "' not valid" << endl;
			cerr << "    Use ./command-line curvature --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		timing::time_point begin, end;

		cout << "Preparing mesh..." << endl;

		begin = timing::now();
		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();
		mesh.make_angles_area();
		end = timing::now();
		cout << "    Angles and areas computed in "
			 << timing::elapsed_seconds(begin, end) << " seconds" << endl;

		vector<double> curv;

		cout << "Compute curvature '" << curvature << "'" << endl;

		double min, max;

		begin = timing::now();
		if (curvature == "Gauss") {
			curvature::Gauss(mesh, curv, nt, &min, &max);
		}
		else if (curvature == "mean") {
			curvature::mean(mesh, curv, nt, &min, &max);
		}
		else {
			cerr << "Error: value for curvature type '" << curvature << "' not valid." << endl;
			cerr << "    Use ./command-line curvature --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		end = timing::now();

		cout.setf(ios::fixed);
		cout.precision(8);
		cout << "Curvature computed in "
			 << timing::elapsed_milliseconds(begin,end)
			 << " ms" << endl;
		cout << "Minimum value of curvature: " << min << endl;
		cout << "Maximum value of curvature: " << max << endl;

		if (print_curv) {
			for (int i = 0; i < mesh.n_vertices(); ++i) {
				cout << i << ": " << curv[i] << endl;
			}
		}

		return 0;
	}

} // -- namespace test_algorithms
