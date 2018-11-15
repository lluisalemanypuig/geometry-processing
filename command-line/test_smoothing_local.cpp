#include "test_geoproc.hpp"

// C++ includes
#include <set>

// geoproc includes
#include <geoproc/smoothing/local.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

namespace test_geoproc {

	void local_smoothing_usage() {
		cout << "Local smoothing evaluation" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --print: prints to standard output the coordinates" << endl;
		cout << "        of the input mesh and of the smoothed mesh." << endl;
		cout << "        Default: do not print." << endl;
		cout << endl;
		cout << "    --threads n: specify number of threads." << endl;
		cout << "        Default: 1" << endl;
		cout << endl;
		cout << "    --algorithm: choose the algorithm to evaluate" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * laplacian" << endl;
		cout << "        * bilaplacian" << endl;
		cout << "        * TaubinLM" << endl;
		cout << endl;
		cout << "        Parameters of each algorithm." << endl;
		cout << "        Unless stated otherwise, all of them are mandatory" << endl;
		cout << "        * laplacian:" << endl;
		cout << "            --lambda" << endl;
		cout << "            --it: number of iterations" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		cout << "        * bilaplacian:" << endl;
		cout << "            --lambda" << endl;
		cout << "            --it: number of iterations" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		cout << "        * TaubinLM:" << endl;
		cout << "            --lambda" << endl;
		cout << "            --it: number of iterations" << endl;
		cout << "            --weight-type" << endl;
		cout << "                Allowed values:" << endl;
		cout << "                * uniform" << endl;
		cout << "                * cotangent" << endl;
		cout << endl;
	}

	int test_smoothing_local(int argc, char *argv[]) {
		const set<string> allowed_algorithms({"laplacian", "bilaplacian", "TaubinLM"});

		string mesh_file = "none";
		string alg = "none";
		string weight_type = "none";
		float lambda;
		size_t nt = 1;
		size_t it;

		bool _print = false;
		bool _lambda = false;
		bool _it = false;

		if (argc == 2) {
			local_smoothing_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				local_smoothing_usage();
				return 1;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--print") == 0) {
				_print = true;
			}
			else if (strcmp(argv[i], "--algorithm") == 0) {
				alg = string(argv[i + 1]);
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
			cerr << "    Use ./command-line smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (alg == "none") {
			cerr << "Error: algorithm not specified" << endl;
			cerr << "    Use ./command-line smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (allowed_algorithms.find(alg) == allowed_algorithms.end()) {
			cerr << "Error: value '" << alg << "' for algorithm parameter not valid" << endl;
			cerr << "    Use ./command-line smoothing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		if (alg == "laplacian" or alg == "bilaplacian" or alg == "TaubinLM") {
			if (not _lambda) {
				cerr << "Error: lambda parameter missing" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return 1;
			}
			if (not _it) {
				cerr << "Error: amount of iterations parameter missing" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return 1;
			}
			if (weight_type == "none") {
				cerr << "Error: weight type parameter missing" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return 1;
			}
			if (weight_type != "uniform" and weight_type != "cotangent") {
				cerr << "Error: value '" << weight_type << "' for weight type not allowed" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return 1;
			}
		}

		smoothing::smooth_weight w;
		if (weight_type != "none") {
			if (weight_type == "uniform") {
				w = smoothing::smooth_weight::uniform;
			}
			else if (weight_type == "cotangent") {
				w = smoothing::smooth_weight::cotangent;
			}
		}

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

		timing::time_point begin = timing::now();
		if (alg == "laplacian") {
			smoothing::local::laplacian(w, lambda, it, nt, mesh);
		}
		else if (alg == "bilaplacian") {
			smoothing::local::bilaplacian(w, lambda, it, nt, mesh);
		}
		else if (alg == "TaubinLM") {
			smoothing::local::TaubinLM(w, lambda, it, nt, mesh);
		}
		timing::time_point end = timing::now();
		cout << "Smoothed mesh in "
			 << timing::elapsed_milliseconds(begin,end)
			 << " milliseconds" << endl;

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
