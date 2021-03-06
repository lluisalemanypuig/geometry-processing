/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

// C includes
#include <string.h>

// C++ includes
#include <iostream>
#include <cmath>
using namespace std;

// glm includes
#include <glm/vec2.hpp>

// geoproc includes
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/iterators/vertex_iterators.hpp>
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/ply_reader.hpp>
using namespace geoproc;
using namespace parametrisation;

// custom includes
#include "time.hpp"

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
		cout << "    --weight-type" << endl;
		cout << "        Allowed values:" << endl;
		cout << "        * uniform" << endl;
		cout << "        * cotangent" << endl;
		cout << endl;
	}

#define out_double(x) ((std::abs(static_cast<double>(x)) <= 1e-4 ? 0.0 : x))
#define geogebra_out(v) "Point({" << out_double(v.x) << "," << out_double(v.y) << "})"

	void print_segments(const TriangleMesh& mesh, const vector<glm::vec2d>& uvs) {
		cout << "{";

		iterators::vertex::vertex_vertex_iterator it(mesh);
		const int N = mesh.n_vertices();
		for (int i = 0; i < N; ++i) {
			it.init(i);

			int first = it.current();
			int j = it.current();
			do {
				cout << "Segment("
					 << geogebra_out(uvs[i]) << ","
					 << geogebra_out(uvs[j])
					 << "),";

				j = it.next();
			}
			while (j != first);
		}
		cout << "}" << endl;
	}

	int test_harmonic_maps(int argc, char *argv[]) {
		string mesh_file = "none";
		string shape_name = "none";
		string weight_type = "none";

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
			else if (strcmp(argv[i], "--weight-type") == 0) {
				weight_type = string(argv[i + 1]);
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
		if (weight_type == "none") {
			cerr << "Error: weight type not specified" << endl;
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

		weight w;
		if (weight_type == "uniform") {
			w = weight::uniform;
		}
		else if (weight_type == "cotangent") {
			w = weight::cotangent;
		}
		else {
			cerr << "Error: value for weight type '" << weight_type << "' not valid." << endl;
			cerr << "    Use ./command-line harmonic-maps --help" << endl;
			cerr << "to see the usage" << endl;
			return 1;
		}

		TriangleMesh mesh;
		PLY_reader::read_mesh(mesh_file, mesh);
		mesh.make_neighbourhood_data();
		mesh.make_boundaries();
		mesh.make_angles_area();

		vector<glm::vec2d> uvs;

		timing::time_point begin = timing::now();
		bool r = harmonic_maps(mesh, w, shape, uvs);
		timing::time_point end = timing::now();

		if (not r) {
			cerr << "Error: some error occured in the harmonic_map function" << endl;
			return 1;
		}

		cout << "Texture coordinates:" << endl;
		cout << "{" << geogebra_out(uvs[0]);
		for (size_t i = 1;  i < uvs.size(); ++i) {
			cout << "," << geogebra_out(uvs[i]);
		}
		cout << "}" << endl;
		print_segments(mesh, uvs);
		cout << "Computed in: " << timing::elapsed_seconds(begin, end) << " s" << endl;

		const int N = mesh.n_vertices();
		vector<bool> constant(N, false);
		for (size_t i = 0; i < mesh.get_boundaries()[0].size(); ++i) {
			int vertex_index = mesh.get_boundaries()[0][i];
			constant[ vertex_index ] = true;
		}

		if (uvs.size() < 20) {
			cout << "Texture coordinates per vertex:" << endl;
			for (size_t i = 0;  i < uvs.size(); ++i) {
				if (constant[i]) {
					cout << "   *";
				}
				else {
					cout << "    ";
				}
				cout << i << ": " << uvs[i].x << "," << uvs[i].y << endl;
			}
		}

		return 0;
	}

} // -- namespace test_algorithms
