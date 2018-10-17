#include "test_algorithms.hpp"

namespace test_algorithms {

	void curvature_usage() {
		cout << "Curvature evaluation" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --curv K: specify the type of curvature to be evaluated" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        Gauss: to evaluate the Gauss curvature Kg" << endl;
		cout << "        mean: to evaluate the mean curvature Kh" << endl;
		cout << endl;
		cout << "    --threads n: specify number of threads." << endl;
		cout << "        Default: 1" << endl;
		cout << endl;
	}

	void test_curvature(int argc, char *argv[]) {
		string mesh_file = "none";
		string curvature = "none";
		size_t nt = 1;

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				curvature_usage();
				return;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--curv") == 0) {
				curvature = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--threads") == 0) {
				nt = atoi(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line curvature --help to see the usage" << endl;
			return;
		}
		if (curvature == "none") {
			cerr << "Error: curvature to be evaluated not specified" << endl;
			cerr << "    Use ./command-line curvature --help to see the usage" << endl;
			return;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();

		timing::time_point begin, end;

		vector<float> curv_mesh, curv_func;
		if (curvature == "Gauss") {
			begin = timing::now();
			algorithms::curvature::Gauss(mesh, curv_func, nt);
			end = timing::now();
		}
		else if (curvature == "mean") {
			begin = timing::now();
			mesh.compute_Kh(curv_mesh);
			end = timing::now();
			cout << "Curvature computed in "
				 << timing::elapsed_milliseconds(begin,end)
				 << " milliseconds" << endl;

			begin = timing::now();
			algorithms::curvature::mean(mesh, curv_func, nt);
			end = timing::now();
		}

		cout << "Curvature computed in "
			 << timing::elapsed_milliseconds(begin,end)
			 << " milliseconds" << endl;

		for (size_t i = 0; i < mesh.n_vertices(); ++i) {
			if (curv_mesh[i] != curv_func[i]) {
				cout << "ERROR (" << i << "):" << endl;
				cout << "     mesh: " << curv_mesh[i] << endl;
				cout << "     func: " << curv_func[i] << endl;
			}
		}
	}

} // -- namespace test_algorithms
