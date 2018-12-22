// C includes
#include <string.h>

// C++ includes
#include <iostream>
using namespace std;

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;

namespace test_geoproc {

	void iterate_usage() {
		cout << "Iteration classes" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --index v: specify vertex index around which the" << endl;
		cout << "        program will iterate" << endl;
		cout << endl;
		cout << "    --vertex-vertex : use vertex-vertex iteration" << endl;
		cout << "    --vertex-face   : use vertex-face iteration" << endl;
		cout << endl;
	}

	int test_iterate(int argc, char *argv[]) {
		string mesh_file = "none";
		string iteration_type = "none";
		int v_idx = -1;

		if (argc == 2) {
			iterate_usage();
			return 1;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				iterate_usage();
				return 0;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--index") == 0) {
				v_idx = atoi(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--vertex-vertex") == 0) {
				iteration_type = "vertex-vertex";
			}
			else if (strcmp(argv[i], "--vertex-face") == 0) {
				iteration_type = "vertex-face";
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line iterate --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (iteration_type == "none") {
			cerr << "Error: iteration type not specified" << endl;
			cerr << "    Use ./command-line iterate --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();

		string current_elem = "none";

		iterators::mesh_iterator *it = nullptr;
		if (iteration_type == "vertex-vertex") {
			it = new iterators::vertex::vertex_vertex_iterator(mesh);
			current_elem = "vertex";
		}
		else if (iteration_type == "vertex-face") {
			it = new iterators::vertex::vertex_face_iterator(mesh);
			current_elem = "face";
		}

		int first = it->init(v_idx);

		// first element
		int v = it->current();
		do {
			cout << "** Current " << current_elem << ": " << v << endl;
			if (v != -1) {
				if (iteration_type == "vertex-face") {
					int v0,v1,v2;
					mesh.get_vertices_triangle(it->current(), v_idx, v0,v1,v2);

					cout << "        vertices: "
						 << v0 << "," << v1 << "," << v2 << endl;
				}
			}
			int k = it->next();
			v = k;

			char step;
			cout << "Enter character:";
			cin >> step;
		}
		while (v != first);

		delete it;

		return 0;
	}

} // -- namespace test_algorithms
