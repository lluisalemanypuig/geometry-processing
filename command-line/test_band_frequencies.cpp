#include "test_geoproc.hpp"

// C++ includes
#include <set>

// geoproc includes
#include <geoproc/filter_frequencies/band_frequencies.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;
using namespace filter_frequencies;

namespace test_geoproc {

	void band_frequencies_usage() {
		cout << "Band frequencies evaluation:" << endl;
		cout << "Apply several smoothing configurations to a mesh" << endl;
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
		cout << "    Since each configuration requires a lot of parameters," << endl;
		cout << "    their parameters are read from standard input." << endl;
		cout << "    The format for a single smoothing algorithm is the following:" << endl;
		cout << "    {                    -- start a new smoothing algorithm" << endl;
		cout << "        lambda l         -- smothing factor" << endl;
		cout << "        it n             -- iterations of the algorithm" << endl;
		cout << "        operator op      -- smoothing operator" << endl;
		cout << "            laplacian,   -- Laplacian smoothing operator" << endl;
		cout << "            bilaplacian, -- Bi-Laplacian smoothing operator" << endl;
		cout << "            TaubinLM     -- Taubin Lambda-Mu operator" << endl;
		cout << "        weight-type w    -- type of weight used" << endl;
		cout << "            uniform,     -- uniform weights" << endl;
		cout << "            cotangent    -- cotangent weights" << endl;
		cout << "    }" << endl;
		cout << "    mu m                 -- the weight for a band frequency" << endl;
		cout << endl;
		cout << "    The weights of the band frequencies should be given after each" << endl;
		cout << "    smoothing configuration, exceptuating the last one." << endl;
		cout << "    The first value of 'mu' found correspond to the weight of the" << endl;
		cout << "    first smoothing configuration, the second to the second, and so on." << endl;
		cout << "    When the whole list has been given, enter" << endl;
		cout << "        -1" << endl;
	}

	bool read_smooth_conf(smoothing_configuration& S, float& MU) {
		char lcurlbracket;
		cin >> lcurlbracket;

		bool lambda = false;
		bool it = false;
		bool oper = false;
		bool weight = false;
		bool mu = false;

		string option;
		while (cin >> option and option != "}") {
			if (option == "lambda") {
				if (lambda) {
					cerr << "Warning: lambda already entered." << endl;
				}
				cin >> S.lambda;
				lambda = true;
			}
			else if (option == "mu") {
				if (mu) {
					cerr << "Warning: mu already entered." << endl;
				}
				cin >> MU;
				mu = true;
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

	void exe_band_freq(size_t nt, TriangleMesh& m) {

		cout << "Describe the band frequencies used:" << endl;
		cout << "Remember that these consist of a smoothing configuration" << endl;
		cout << "and a weight value (mu). Recall the format:" << endl;
		cout << "    {                    -- start a new smoothing algorithm" << endl;
		cout << "        lambda l         -- smothing factor" << endl;
		cout << "        it n             -- iterations of the algorithm" << endl;
		cout << "        operator op      -- smoothing operator" << endl;
		cout << "            laplacian,   -- Laplacian smoothing operator" << endl;
		cout << "            bilaplacian, -- Bi-Laplacian smoothing operator" << endl;
		cout << "            TaubinLM     -- Taubin Lambda-Mu operator" << endl;
		cout << "        weight-type w    -- type of weight used" << endl;
		cout << "            uniform,     -- uniform weights" << endl;
		cout << "            cotangent    -- cotangent weights" << endl;
		cout << "    }" << endl;
		cout << "    mu m                 -- the weight for a band frequency" << endl;
		cout << endl;
		cout << "    The weights of the band frequencies should be given after each" << endl;
		cout << "    smoothing configuration, exceptuating the last one." << endl;

		timing::time_point begin, end;

		vector<smoothing_configuration> Cs;
		vector<float> mus;

		bool keep_reading = true;
		while (keep_reading) {
			float mu;
			smoothing_configuration C;
			if (read_smooth_conf(C, mu)) {
				Cs.push_back(C);
				cin >> std::ws;
				int c = cin.peek();
				keep_reading = (char(c) != '-');
			}
			if (keep_reading) {
				mus.push_back(mu);
			}
		}

		// read trailing '1' character
		char one; cin >> one;

		begin = timing::now();
		band_frequencies(Cs, mus, nt, m);
		end = timing::now();

		cout << "Applied band frequencies in "
			 << timing::elapsed_milliseconds(begin, end) << " ms" << endl;
	}

	int test_band_frequency(int argc, char *argv[]) {

		string mesh_file = "none";
		size_t nt = 1;

		bool _print = false;

		if (argc == 2) {
			band_frequencies_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				band_frequencies_usage();
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
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line band-frequencies --help" << endl;
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

		exe_band_freq(nt, mesh);

		if (_print) {
			cout << "Mesh after applying band frequencies:" << endl;
			for (int i = 0; i < mesh.n_vertices(); ++i) {
				const vec3& v = mesh.get_vertex(i);
				cout << "    " << i << ": "
					 << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		return 0;
	}

} // -- namespace test_algorithms
