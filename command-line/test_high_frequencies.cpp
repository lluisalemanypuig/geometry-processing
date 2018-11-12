#include "test_geoproc.hpp"

// C++ includes
#include <set>

// geoproc includes
#include <geoproc/high-frequencies/highfrequencies.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;
using namespace high_frequencies;

namespace test_geoproc {

	void high_frequencies_usage() {
		cout << "High frequencies evaluation" << endl;
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
		cout << "        * high-freq-dets" << endl;
		cout << "        * exaggerate-high-freqs" << endl;
		cout << "        * band-freqs" << endl;
		cout << endl;
		cout << "        Since each algorithm requires a lot of parameters" << endl;
		cout << "        their configuration is read from standard input." << endl;
		cout << "        Basically, each of them need one smoothing algorithm," << endl;
		cout << "        at least. The format for a single smoothing algorithm is" << endl;
		cout << "        following:" << endl;
		cout << "        {                    -- start a new smoothing algorithm" << endl;
		cout << "            lambda l         -- smothing factor" << endl;
		cout << "            it n             -- iterations of the algorithm" << endl;
		cout << "            operator op      -- smoothing operator" << endl;
		cout << "                laplacian,   -- Laplacian smoothing operator" << endl;
		cout << "                bilaplacian, -- Bi-Laplacian smoothing operator" << endl;
		cout << "                TaubinLM     -- Taubin Lambda-Mu operator" << endl;
		cout << "            weight-type w    -- type of weight used" << endl;
		cout << "                uniform,     -- uniform weights" << endl;
		cout << "                cotangent    -- cotangent weights" << endl;
		cout << "        }" << endl;
		cout << "        The specification of the different smoothing algorithms" << endl;
		cout << "        is done by simply writing all of them following the previous" << endl;
		cout << "        format. When the whole list has been given, enter" << endl;
		cout << "            -1" << endl;
	}

	bool read_smooth_conf(smoothing_configuration& S) {
		char lcurlbracket;
		cin >> lcurlbracket;

		bool lambda = false;
		bool it = false;
		bool oper = false;
		bool weight = false;

		string option;
		while (cin >> option and option != "}") {
			if (option == "lambda") {
				if (lambda) {
					cerr << "Warning: lambda already entered." << endl;
				}
				cin >> S.lambda;
				lambda = true;
			}
			else if (option == "it") {
				if (it) {
					cerr << "Warning: number of iterations already entered." << endl;
				}
				cin >> S.n_it;
				it = true;
			}
			else if (option == "operator") {
				string o;
				cin >> o;
				if (o == "laplacian") {
					S.so = smoothing::smooth_operator::Laplacian;
					oper = true;
				}
				else if (o == "laplacian") {
					S.so = smoothing::smooth_operator::BiLaplacian;
					oper = true;
				}
				else if (o == "laplacian") {
					S.so = smoothing::smooth_operator::TaubinLM;
					oper = true;
				}
				else {
					cerr << "Error: smoothing operator type '" << o << "' is not valid." << endl;
				}
			}
			else if (option == "weight-type") {
				if (weight) {
					cerr << "Warning: weight already entered." << endl;
				}
				string w;
				cin >> w;
				if (w == "uniform") {
					S.sw = smoothing::smooth_weight::uniform;
					weight = true;
				}
				else if (w == "cotangent") {
					S.sw = smoothing::smooth_weight::cotangent;
					weight = true;
				}
				else {
					cerr << "Error: weight type '" << w << "' is not valid." << endl;
				}
			}
			else {
				cerr << "Error: option '" << option << "' is not valid." << endl;
			}
		}

		if (not lambda) {
			cerr << "Error: lambda parameter not entered." << endl;
		}
		if (not it) {
			cerr << "Error: number of iterations parameter not entered." << endl;
		}
		if (not oper) {
			cerr << "Error: smoothing operator not entered." << endl;
		}
		if (not weight) {
			cerr << "Error: weight type parameter not entered." << endl;
		}
		if (not lambda or not it or not weight or not oper) {
			cerr << "Error: missing parameters. Configuration will be discarded" << endl;
		}
		return lambda and it and weight and oper;
	}

	void exe_high_freq(const string& alg, TriangleMesh& m) {

		if (alg == "high_freq_dets") {
			smoothing_configuration C;
			if (read_smooth_conf(C)) {
				high_frequencies::high_frequency_details(C, m);
			}
		}
		else if (alg == "exaggerate-high-freqs") {
			smoothing_configuration C;
			if (read_smooth_conf(C)) {
				high_frequencies::exaggerate_high_frequencies(C, m);
			}
		}
		else if (alg == "band-freqs") {
			vector<smoothing_configuration> Cs;
			bool keep_reading = true;
			while (keep_reading) {
				smoothing_configuration C;
				if (read_smooth_conf(C)) {
					Cs.push_back(C);
					cin >> std::ws;
					int c = cin.peek();
					keep_reading = (char(c) != '-');
				}
			}

			// read trailing '1' character
			char one; cin >> one;

			high_frequencies::band_frequencies(Cs, m);
		}

	}

	int test_high_frequency(int argc, char *argv[]) {
		const set<string> allowed_algorithms(
		{"high-freq-dets", "exaggerate-high-freqs", "band-freqs"});

		string alg = "none";
		string mesh_file = "none";
		size_t nt = 1;

		bool _print = false;

		if (argc == 2) {
			high_frequencies_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				high_frequencies_usage();
				return 1;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--print") == 0) {
				_print = true;
			}
			else if (strcmp(argv[i], "--threads") == 0) {
				nt = atoi(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--algorithm") == 0) {
				alg = string(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line high-frequency --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		if (allowed_algorithms.find(alg) == allowed_algorithms.end()) {
			cerr << "Error: value '" << alg << "' for algorithm parameter not valid" << endl;
			cerr << "    Use ./command-line high-frequency --help" << endl;
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
				const vec3& v = mesh.get_vertex(i);
				cout << "    " << i << ": "
					 << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		exe_high_freq(alg, mesh);

		if (_print) {
			cout << "Mesh with high frequencies:" << endl;
			for (int i = 0; i < mesh.n_vertices(); ++i) {
				const vec3& v = mesh.get_vertex(i);
				cout << "    " << i << ": "
					 << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		return 0;
	}

} // -- namespace test_algorithms
