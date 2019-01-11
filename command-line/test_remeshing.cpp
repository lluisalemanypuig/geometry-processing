// C includes
#include <string.h>

// C++ includes
#include <iostream>
#include <set>
using namespace std;

// glm includes
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/remeshing/remeshing.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;
using namespace parametrisation;

// custom includes
#include "time.hpp"

namespace test_geoproc {

	void remeshing_harmonic_maps_usage() {
		cout << "Remeshing evaluation:" << endl;
		cout << "Apply several smoothing configurations to a mesh" << endl;
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "    --N n: number of points in the x-axis" << endl;
		cout << endl;
		cout << "    --M m: number of points in the y-axis" << endl;
		cout << endl;
		cout << "    --shape c: shape of the boundary vertices on the texture" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * square" << endl;
		cout << endl;
		cout << "    --weight-type" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * uniform" << endl;
		cout << "        * cotangent" << endl;
		cout << endl;
	}

	int test_remeshing_harmonic_maps(int argc, char *argv[]) {
		string mesh_file = "none";
		string shape_name = "none";
		string weight_type = "none";
		size_t N = 0;
		size_t M = 0;

		if (argc == 2) {
			remeshing_harmonic_maps_usage();
			return 0;
		}

		for (int i = 2; i < argc; ++i) {
			if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
				remeshing_harmonic_maps_usage();
				return 1;
			}
			else if (strcmp(argv[i], "--load") == 0) {
				mesh_file = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--N") == 0) {
				N = atoi(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--M") == 0) {
				M = atoi(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--shape") == 0) {
				shape_name = string(argv[i + 1]);
				++i;
			}
			else if (strcmp(argv[i], "--weight-type") == 0) {
				weight_type = string(argv[i + 1]);
				++i;
			}
			else {
				cerr << "Error: option '" << string(argv[i])
					 << "' not recognised" << endl;
				return 1;
			}
		}

		if (mesh_file == "none") {
			cerr << "Error: mesh file not specified" << endl;
			cerr << "    Use ./command-line remeshing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (shape_name == "none") {
			cerr << "Error: shape for boundary vertices not specified" << endl;
			cerr << "    Use ./command-line remeshing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (weight_type == "none") {
			cerr << "Error: weight type not specified" << endl;
			cerr << "    Use ./command-line remeshing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (N == 0) {
			cerr << "Error: parameter N not specified" << endl;
			cerr << "    Use ./command-line remeshing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}
		if (M == 0) {
			cerr << "Error: parameter M not specified" << endl;
			cerr << "    Use ./command-line remeshing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		weight w;
		if (weight_type == "uniform") {
			w = weight::uniform;
		}
		else if (weight_type == "cotangent") {
			w = weight::cotangent;
		}
		else {
			cerr << "Error: value for weight type '" << weight_type
				 << "' not valid." << endl;
			cerr << "    Use ./command-line remeshing --help" << endl;
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
			cerr << "    Use ./command-line remeshing --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		TriangleMesh mesh, new_mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();
		mesh.make_boundaries();
		mesh.make_angles_area();

		bool r;
		timing::time_point begin = timing::now();
		r = remeshing::harmonic_maps(mesh, N,M, w, shape, new_mesh);
		timing::time_point end = timing::now();
		cout << "Remeshed in " << timing::elapsed_seconds(begin, end)
			 << " seconds" << endl;

		if (not r) {
			cerr << "Error: some error occured. Aborting."
				 << endl;
			return 1;
		}

		new_mesh.make_normal_vectors();
		new_mesh.make_neighbourhood_data();
		new_mesh.make_boundaries();
		new_mesh.make_angles_area();

		const vector<glm::vec3>& new_verts = new_mesh.get_vertices();
		if (new_verts.size() <= 30) {
			cout << "Vertices of the new mesh:" << endl;
			for (const glm::vec3& v : new_verts) {
				cout << "    " << v.x << "," << v.y << "," << v.z << endl;
			}
		}

		return 0;
	}

} // -- namespace test_algorithms
