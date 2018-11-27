#include "test_geoproc.hpp"

// C++ includes
#include <set>

// geoproc includes
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;
using namespace parametrisation;

namespace test_geoproc {

	void harmonic_maps_usage() {
		cout << "Harmonic maps evaluation:" << endl;
		cout << "Apply several smoothing configurations to a mesh" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --shape c: shape of the boundary vertices on the texture" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * circle" << endl;
		cout << "        * square" << endl;
		cout << endl;
	}

	int test_harmonic_maps(int argc, char *argv[]) {
		string mesh_file = "none";
		string shape_name = "none";

		if (argc == 2) {
			harmonic_maps_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				harmonic_maps_usage();
				return 1;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--shape") == 0) {
				shape_name = string(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i]) << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line harmonic-maps --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (shape_name == "none") {
			cerr << "Error: shape for boundary vertices not specified" << endl;
			cerr << "    Use ./command-line harmonic-maps --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		boundary_shape shape;
		if (shape_name == "circle") {
			shape = boundary_shape::Circle;
		}
		else if (shape_name == "square") {
			shape = boundary_shape::Square;
		}
		else {
			cerr << "Error: value for shape of boundary vertices '"
				 << shape_name << "' not valid." << endl;
			cerr << "    Use ./command-line harmonic-maps --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();
		mesh.make_boundaries();
		mesh.make_angles_area();

		vector<glm::vec2> uvs;

		timing::time_point begin = timing::now();
		harmonic_maps(mesh, shape, uvs);
		timing::time_point end = timing::now();

		cout << "Texture coordinates:" << endl;
		for (size_t i = 0;  i < uvs.size(); ++i) {
			cout << "    " << i << ": (" << uvs[i].x << "," << uvs[i].y << ")" << endl;
		}
		cout << "Computed in: " << timing::elapsed_seconds(begin, end) << " s" << endl;

		return 0;
	}

} // -- namespace test_algorithms
