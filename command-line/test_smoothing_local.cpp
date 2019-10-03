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
#include <set>
using namespace std;

// glm includes
#include <glm/vec3.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/smoothing/local.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

// custom includes
#include "time.hpp"

namespace test_geoproc {

	void smoothing_local_usage() {
		cout << "Local smoothing evaluation" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --print : prints to standard output the coordinates" << endl;
		cout << "        of the input mesh and of the smoothed mesh." << endl;
		cout << "        Default: do not print." << endl;
		cout << endl;
		cout << "    --threads n: specify number of threads." << endl;
		cout << "        Default: 1" << endl;
		cout << endl;
		cout << "    --operator : choose the operator to apply" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * laplacian" << endl;
		cout << "        * bilaplacian" << endl;
		cout << "        * TaubinLM" << endl;
		cout << endl;
		cout << "        Parameters of each algorithm." << endl;
		cout << "        Unless stated otherwise, all of them are mandatory" << endl;
		cout << "        * laplacian :" << endl;
		cout << "            --lambda : weight for each difference" << endl;
		cout << "            --it : number of iterations" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		cout << "        * bilaplacian :" << endl;
		cout << "            --lambda : weight for each difference" << endl;
		cout << "            --it : number of iterations" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		cout << "        * TaubinLM :" << endl;
		cout << "            --lambda : weight for each difference" << endl;
		cout << "            --it : number of iterations" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		cout << endl;
	}

	int test_smoothing_local(int argc, char *argv[]) {
		string mesh_file = "none";
		string opt = "none";
		string weight_type = "none";
		double lambda = 0.5;
		size_t nt = 1;
		size_t it = 1;

		bool _print = false;
		bool _lambda = false;
		bool _it = false;

		if (argc == 2) {
			smoothing_local_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				smoothing_local_usage();
				return 1;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--print") == 0) {
				_print = true;
			}
			else if (strcmp(argv[i], "--operator") == 0) {
				opt = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--lambda") == 0) {
				lambda = atof(argv[i + 1]);
				_lambda = true;
				++i;
			}
			else if (strcmp(argv[i], "--it") == 0) {
				it = atof(argv[i + 1]);
				_it = true;
				++i;
			}
			else if (strcmp(argv[i], "--weight-type") == 0) {
				weight_type = string(argv[i + 1]);
				++i;
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
			cerr << "    Use ./command-line local-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (opt == "none") {
			cerr << "Error: operator not specified" << endl;
			cerr << "    Use ./command-line local-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (weight_type == "none") {
			cerr << "Error: weight type not specified" << endl;
			cerr << "    Use ./command-line local-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		if (opt == "laplacian" or opt == "bilaplacian" or opt == "TaubinLM") {
			if (not _lambda) {
				cerr << "Error: lambda parameter missing" << endl;
				cerr << "    Use ./command-line local-smoothing --help" << endl;
				cerr << "to see the usage" << endl;
				return 1;
			}
			if (not _it) {
				cerr << "Error: amount of iterations parameter missing" << endl;
				cerr << "    Use ./command-line local-smoothing --help" << endl;
				cerr << "to see the usage" << endl;
				return 1;
			}
			if (weight_type == "none") {
				cerr << "Error: weight type parameter missing" << endl;
				cerr << "    Use ./command-line local-smoothing --help" << endl;
				cerr << "to see the usage" << endl;
				return 1;
			}
			if (weight_type != "uniform" and weight_type != "cotangent") {
				cerr << "Error: value '" << weight_type << "' for weight type not allowed" << endl;
				cerr << "    Use ./command-line local-smoothing --help" << endl;
				cerr << "to see the usage" << endl;
				return 1;
			}
		}
		else {
			cerr << "Error: value for operator '" << opt << "' not valid." << endl;
			cerr << "    Use ./command-line global-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		weight w;
		if (weight_type == "uniform") {
			w = weight::uniform;
		}
		else if (weight_type == "cotangent") {
			w = weight::cotangent;
		}
		else {
			cerr << "Error: value for weight type '" << weight_type << "' not valid" << endl;
			cerr << "    Use ./command-line local-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();
		mesh.make_angles_area();

		if (_print) {
			cout << "Loaded mesh:" << endl;
			for (int i = 0; i < mesh.n_vertices(); ++i) {
				const vec3d& v = mesh.get_vertex(i);
				cout << "    " << i << ": "
					 << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		timing::time_point begin = timing::now();
		if (opt == "laplacian") {
			smoothing::local::laplacian(w, lambda, it, nt, mesh);
		}
		else if (opt == "bilaplacian") {
			smoothing::local::bilaplacian(w, lambda, it, nt, mesh);
		}
		else if (opt == "TaubinLM") {
			smoothing::local::TaubinLM(w, lambda, it, nt, mesh);
		}
		timing::time_point end = timing::now();
		cout << "Smoothed mesh in "
			 << timing::elapsed_milliseconds(begin,end)
			 << " ms" << endl;

		if (_print) {
			cout << "Smoothed mesh:" << endl;
			for (int i = 0; i < mesh.n_vertices(); ++i) {
				const vec3& v = mesh.get_vertex(i);
				cout << "    " << i << ": "
					 << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		return 0;
	}

} // -- namespace test_algorithms
