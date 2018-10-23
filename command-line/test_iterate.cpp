#include "test_algorithms.hpp"

namespace test_algorithms {

	void iterate_usage() {
		cout << "Iteration classes" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --index v: specify vertex index around which the" << endl;
		cout << "        program will iterate" << endl;
		cout << endl;
		cout << "    --vertex-vertex: use vertex-vertex iteration" << endl;
		cout << "    --vertex-face:   use vertex-face iteration" << endl;
		cout << endl;
	}

	void test_iterate(int argc, char *argv[]) {
		string mesh_file = "none";
		string iteration_type = "none";
		int v_idx = -1;

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				iterate_usage();
				return;
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
				return;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line iterate --help to see the usage" << endl;
			return;
		}
		if (iteration_type == "none") {
			cerr << "Error: iteration type not specified" << endl;
			cerr << "    Use ./command-line iterate --help to see the usage" << endl;
			return;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();

		iterators::mesh_iterator *it = nullptr;
		if (iteration_type == "vertex-vertex") {
			it = new iterators::vertex::vertex_vertex_iterator(mesh);
		}
		else if (iteration_type == "vertex-face") {
			it = new iterators::vertex::vertex_face_iterator(mesh);
		}

		it->init(v_idx);

		// first element
		int first = it->current();
		do {
			cout << "    " << it->current() << endl;
			if (iteration_type == "vertex-face") {
				int v0,v1,v2;
				mesh.get_vertices_face(it->current(), v_idx, v0,v1,v2);

				cout << "        vertices: "
					 << v0 << "," << v1 << "," << v2 << endl;
			}
		}
		while (it->next() != first);
	}

} // -- namespace test_algorithms