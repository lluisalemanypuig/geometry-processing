// C++ includes
#include <iostream>
#include <iomanip>
using namespace std;

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

// custom includes
#include "time.hpp"

namespace test_geoproc {

	void inspect_usage() {
		cout << "Triangle mesh inspection." << endl;
		cout << "Load a triangle mesh and inspect it." << endl;
		cout << "Outputs the following information:" << endl;
		cout << endl;
		cout << "    -> Number of vertices" << endl;
		cout << "    -> Number of triangles" << endl;
		cout << "    -> Number of corners" << endl;
		cout << "    -> Number of edges" << endl;
		cout << "    -> Number of boundary edges" << endl;
		cout << "    -> Number of boundaries" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
	}

#define vertex_id(c) mesh.get_vertex_corner(c)
#define edge_corner_out(e)				\
	"(" << e.v0 << "," << e.v1 << ")"
#define edge_vertex_out(e)				\
	"(" << vertex_id(e.v0)				\
	<< "," << vertex_id(e.v1) << ")"

	int test_inspect(int argc, char *argv[]) {
		string mesh_file = "none";

		if (argc == 2) {
			inspect_usage();
			return 1;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				inspect_usage();
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
			cerr << "    Use ./command-line inspect --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		timing::time_point begin = timing::now();

		TriangleMesh mesh;
		bool r = PLY_reader::read_mesh(mesh_file, mesh);
		if (not r) {
			return 1;
		}

		mesh.make_neighbourhood_data();
		mesh.make_angles_area();
		mesh.make_boundaries();

		int max_length = 20;

		cout << setw(max_length)
			 << "# vertices:" << mesh.n_vertices() << endl;
		cout << setw(max_length)
			 << "# triangles: " << mesh.n_triangles() << endl;
		cout << setw(max_length)
			 << "# corners: " << mesh.n_corners() << endl;
		cout << setw(max_length)
			 << "# edges: " << mesh.n_edges() << endl;
		cout << setw(max_length)
			 << "# boundary edges: " << mesh.n_boundary_edges() << endl;
		cout << setw(max_length)
			 << "# boundaries: " << mesh.n_boundaries() << endl;

		timing::time_point end = timing::now();
		cout << "All this in: " << timing::elapsed_seconds(begin, end)
			 << " seconds" << endl;

		return 0;
	}

} // -- namespace test_algorithms