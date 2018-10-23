#include "test_algorithms.hpp"

namespace test_algorithms {

	void smoothing_usage() {
		cout << "Smoothing evaluation" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --algorithm: choose the algorithm to evaluate" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * laplacian" << endl;
		cout << endl;
		cout << "    Parameters of each algorithm." << endl;
		cout << "    Unless stated otherwise, all of them are mandatory" << endl;
		cout << "    * laplacian:" << endl;
		cout << "        --lambda" << endl;
		cout << "        --it: number of iterations" << endl;
		cout << "        --weight-type" << endl;
		cout << "            Allowed values:" << endl;
		cout << "            * uniform" << endl;
		cout << "            * cotangent" << endl;
		cout << endl;
	}

	void test_smoothing(int argc, char *argv[]) {
		string mesh_file = "none";
		string alg = "none";
		float lambda;
		size_t it;
		algorithms::smoothing::smooth_weight w;

		bool _lambda = false;
		bool _w = false;
		bool _it = false;

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				smoothing_usage();
				return;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
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
				if (strcmp(argv[i + 1], "uniform") == 0) {
					w = algorithms::smoothing::smooth_weight::uniform;
				}
				else if (strcmp(argv[i + 1], "cotangent") == 0) {
					w = algorithms::smoothing::smooth_weight::cotangent;
				}
				else {
					cerr << "Value '" << string(argv[i + 1]) << "' for weight type not allowed" << endl;
					return;
				}

				_w = true;
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
			return;
		}
		if (alg == "none") {
			cerr << "Error: algorithm not specified" << endl;
			cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
			return;
		}
		if (alg != "laplacian") {
			cerr << "Error: value '" << alg << "' for algorithm parameter not valid" << endl;
			cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
			return;
		}

		if (alg == "laplacian") {
			if (not _lambda) {
				cerr << "Error: lambda parameter missing" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return;
			}
			if (not _it) {
				cerr << "Error: amount of iterations parameter missing" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return;
			}
			if (not _w) {
				cerr << "Error: weight type parameter missing" << endl;
				cerr << "    Use ./command-line smoothing --help to see the usage" << endl;
				return;
			}
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();

		cout << "Loaded mesh:" << endl;
		for (int i = 0; i < mesh.n_vertices(); ++i) {
			const vec3& v = mesh.get_vertex(i);
			cout << "    " << i << ": " << v.x << "," << v.y << "," << v.z << endl;
		}

		if (alg == "laplacian") {
			algorithms::smoothing::laplacian(w, lambda, it, mesh);
		}

		cout << "Smoothed mesh:" << endl;
		for (int i = 0; i < mesh.n_vertices(); ++i) {
			const vec3& v = mesh.get_vertex(i);
			cout << "    " << i << ": " << v.x << "," << v.y << "," << v.z << endl;
		}
	}

} // -- namespace test_algorithms
