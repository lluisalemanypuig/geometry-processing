// C includes
#include <string.h>

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
		cout << endl;
		cout << "    --load f: load a mesh stored in the .ply file f" << endl;
		cout << endl;
		cout << "Outputs the following information:" << endl;
		cout << endl;
		cout << "    - Number of vertices" << endl;
		cout << "    - Number of edges" << endl;
		cout << "    - Number of triangles" << endl;
		cout << "    - Number of corners" << endl;
		cout << "    - Number of edges" << endl;
		cout << "    - Number of boundary edges" << endl;
		cout << "    - Number of boundaries" << endl;
		cout << endl;
	}

#define vertex_id(c) mesh.get_vertex_corner(c)
#define edge_corner_out(e)				\
	"(" << e.v0 << "," << e.v1 << ")"
#define edge_vertex_out(e)				\
	"(" << vertex_id(e.v0)				\
	<< "," << vertex_id(e.v1) << ")"

	inline void print_edge(size_t i, const mesh_edge& e) {
		cout << "    " << setw(4) << i
			 << ": v0: " << e.v0 << endl;

		cout << "    " << setw(4) << " "
			 << "  v1: " << e.v1 << endl;
		cout << "    " << setw(4) << " "
			 << "  fL: " << e.lT << endl;
		cout << "    " << setw(4) << " "
			 << "  fR: " << e.rT << endl;
		cout << "    " << setw(4) << " "
			 << "  pE: " << e.pE << endl;
		cout << "    " << setw(4) << " "
			 << "  nE: " << e.nE << endl;
	}

	inline void print_edges(const vector<mesh_edge>& edges) {
		size_t col_max_len[7] = {0,2,2,2,2,2,2};
		col_max_len[0] =
			std::max(std::to_string(edges.size()).length(), size_t(6));

		for (size_t i = 0; i < edges.size(); ++i) {
			string v0 = std::to_string(edges[i].v0);
			string v1 = std::to_string(edges[i].v1);
			string lT = std::to_string(edges[i].lT);
			string rT = std::to_string(edges[i].rT);
			string pE = std::to_string(edges[i].pE);
			string nE = std::to_string(edges[i].nE);

			col_max_len[1] = std::max(v0.length(), col_max_len[1]);
			col_max_len[2] = std::max(v1.length(), col_max_len[2]);
			col_max_len[3] = std::max(lT.length(), col_max_len[3]);
			col_max_len[4] = std::max(rT.length(), col_max_len[4]);
			col_max_len[6] = std::max(pE.length(), col_max_len[5]);
			col_max_len[5] = std::max(nE.length(), col_max_len[6]);
		}

		// print header
		cout << setw(col_max_len[0]) << "index" << " "
			 << setw(col_max_len[1]) << "v0" << " "
			 << setw(col_max_len[2]) << "v1" << " "
			 << setw(col_max_len[3]) << "lT" << " "
			 << setw(col_max_len[4]) << "rT" << " "
			 << setw(col_max_len[5]) << "pE" << " "
			 << setw(col_max_len[6]) << "nE" << endl;

		// print edges
		for (size_t i = 0; i < edges.size(); ++i) {
			const mesh_edge& E = edges[i];
			cout << setw(col_max_len[0]) << i << " "
				 << setw(col_max_len[1]) << E.v0 << " "
				 << setw(col_max_len[2]) << E.v1 << " "
				 << setw(col_max_len[3]) << E.lT << " "
				 << setw(col_max_len[4]) << E.rT << " "
				 << setw(col_max_len[5]) << E.pE << " "
				 << setw(col_max_len[6]) << E.nE << endl;
		}
	}

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

		timing::time_point end = timing::now();
		cout << "Gathered data in: " << timing::elapsed_seconds(begin, end)
			 << " seconds" << endl;

		int max_length = 20;

		cout << setw(max_length)
			 << "# vertices:" << mesh.n_vertices() << endl;
		cout << setw(max_length)
			 << "# edges:" << mesh.n_edges() << endl;
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

		const vector<int>& edge_vertex = mesh.get_vertex_edge();
		const vector<mesh_edge>& edges = mesh.get_edges();

		// ---------------------------------------------------------------------
		cout << "Vertices:" << endl;
		for (int v = 0; v < mesh.n_vertices(); ++v) {
			const glm::vec3& vtx = mesh.get_vertex(v);
			cout << "    " << v << ": "
				 << "(" << vtx.x << "," << vtx.y << "," << vtx.z << ")"
				 << endl;
			cout << "        incident to edge: " << edge_vertex[v] << endl;

			int e = edge_vertex[v];
			const mesh_edge& mE = edges[e];
			if (mE.v0 != v and mE.v1 != v) {
				cerr << "Error (" << __LINE__ << "):" << endl;
				cerr << "    vertex is related to edge with different endpoints"
					 << endl;
				print_edge(e, mE);
				return 1;
			}
		}

		// ---------------------------------------------------------------------
		cout << "Edges:" << endl;
		print_edges(edges);

		return 0;
	}

} // -- namespace test_algorithms
