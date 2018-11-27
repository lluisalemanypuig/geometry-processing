#include "test_geoproc.hpp"

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

namespace test_geoproc {

	void boundaries_usage() {
		cout << "Boundary computation" << endl;
		cout << "Output the corners that make up a mesh. Then," << endl;
		cout << "output the vertex indices." << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
	}

	int test_boundaries(int argc, char *argv[]) {
		string mesh_file = "none";

		if (argc == 2) {
			boundaries_usage();
			return 1;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				boundaries_usage();
				return 0;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line boundaries --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();

		cout << "This mesh has " << mesh.get_boundary_edges().size()
			 << " boundary edges" << endl;

		timing::time_point begin = timing::now();
		mesh.make_boundaries();
		timing::time_point end = timing::now();
		cout << "Boundaries computed in "
			 << timing::elapsed_milliseconds(begin,end) << " ms" << endl;

		const vector<vector<int> >& bounds = mesh.get_boundaries();
		cout << "This mesh has " << bounds.size() << " boundaries." << endl;
		cout << "Corner indices:" << endl;
		for (size_t i = 0; i < bounds.size(); ++i) {
			cout << "    " << i << ":";
			for (size_t j = 0; j < bounds[i].size(); ++j) {
				cout << " " << bounds[i][j];
			}
			cout << endl;
		}
		cout << "Vertex indices:" << endl;
		for (size_t i = 0; i < bounds.size(); ++i) {
			cout << "    " << i << ":";
			for (size_t j = 0; j < bounds[i].size(); ++j) {
				cout << " " << mesh.get_vertex_corner(bounds[i][j]);
			}
			cout << endl;
		}

		return 0;
	}

} // -- namespace test_algorithms
