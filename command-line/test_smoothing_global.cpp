// C includes
#include <string.h>

// C++ includes
#include <iostream>
#include <numeric>
#include <set>
using namespace std;

// glm includes
#include <glm/vec3.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/smoothing/global.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

// custom includes
#include "time.hpp"

namespace test_geoproc {

	void smoothing_global_usage() {
		cout << "Global smoothing evaluation" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --print : prints to standard output the coordinates" << endl;
		cout << "        of the input mesh and of the smoothed mesh." << endl;
		cout << "        Default: do not print." << endl;
		cout << endl;
		cout << "    --percentage p: percentage of the mesh's vertices to be fixed" << endl;
		cout << "        Default 50.0" << endl;
		cout << "        Optional" << endl;
		cout << endl;
		cout << "    --operator : choose the smoothing operator" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * laplacian" << endl;
		//cout << "        * bilaplacian" << endl;
		cout << endl;
		cout << "        Parameters of each operator." << endl;
		cout << "        Unless stated otherwise, all of them are mandatory" << endl;
		cout << "        * laplacian :" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		/*cout << "        * bilaplacian :" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;*/
		cout << endl;
	}

	int test_smoothing_global(int argc, char *argv[]) {
		const set<string> allowed_operators({"laplacian"});

		string mesh_file = "none";
		string opt = "none";
		string weight_type = "none";
		float perc = 50.0f;

		bool _print = false;

		if (argc == 2) {
			smoothing_global_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				smoothing_global_usage();
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
			else if (strcmp(argv[i], "--percentage") == 0) {
				perc = atof(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--weight-type") == 0) {
				weight_type = string(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line global-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (opt == "none") {
			cerr << "Error: operator not specified" << endl;
			cerr << "    Use ./command-line global-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (weight_type == "none") {
			cerr << "Error: weight type not specified" << endl;
			cerr << "    Use ./command-line global-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		cout << "Smooth globally:" << endl;
		cout << "    with operator: ";

		smoothing::smooth_operator o;
		if (opt == "laplacian") {
			o = smoothing::smooth_operator::Laplacian;
			cout << "Laplacian";
		}
		else {
			cerr << "Error: value for operator '" << opt << "' not valid." << endl;
			cerr << "    Use ./command-line global-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		cout << endl;

		cout << "    with ";
		smoothing::smooth_weight w;
		if (weight_type == "uniform") {
			w = smoothing::smooth_weight::uniform;
			cout << "uniform";
		}
		else if (weight_type == "cotangent") {
			w = smoothing::smooth_weight::cotangent;
			cout << "cotangent";
		}
		else {
			cerr << "Error: value for weight type '" << weight_type << "' not valid." << endl;
			cerr << "    Use ./command-line global-smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		cout << " weights" << endl;
		cout << "    percentage of fixed vertices: " << perc << "%" << endl;

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();
		mesh.make_angles_area();

		if (_print) {
			cout << "Loaded mesh:" << endl;
			for (int i = 0; i < mesh.n_vertices(); ++i) {
				const vec3& v = mesh.get_vertex(i);
				cout << "    " << i << ": "
					 << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		timing::time_point begin, end;

		int N = mesh.n_vertices();
		vector<int> indices(N);
		iota(indices.begin(), indices.end(), 0);
		int max_idx = N - 1;

		vector<bool> constant(N, false);
		while ((100.0f*(N - max_idx - 1))/N < perc) {
			int i = rand()%(max_idx + 1);
			constant[indices[i]] = true;

			swap( indices[i], indices[max_idx] );
			--max_idx;
		}

		begin = timing::now();
		smoothing::global::smooth(o, w, constant, mesh);
		end = timing::now();

		cout << "Smoothed mesh globally in "
			 << timing::elapsed_seconds(begin,end)
			 << " s" << endl;

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
