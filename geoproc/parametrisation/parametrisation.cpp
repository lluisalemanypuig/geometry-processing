#include <geoproc/parametrisation/parametrisation.hpp>

// C++ includes
#include <iostream>
using namespace std;

namespace geoproc {
namespace parametrisation {

	bool harmonic_maps
	(
		const TriangleMesh& m, const boundary_shape& s,
		vector<glm::vec2>& uvs
	)
	{
		const vector<vector<int> >& bounds = m.get_boundaries();
		if (bounds.size() == 0) {
			cerr << "geoproc::parametrisation::harmonic_maps - Error" << endl;
			cerr << "    No boundaries in input mesh." << endl;
			return false;
		}
		if (bounds.size() > 1) {
			cerr << "geoproc::parametrisation::harmonic_maps - Error" << endl;
			cerr << "    Too many boundaries in input mesh." << endl;
			return false;
		}

		const int N = m.n_vertices();
		uvs = vector<glm::vec2>(N, glm::vec2(0.0f,0.0f));

		// boundary contains corner indices
		const vector<int>& boundary = bounds[0];

		if (s == boundary_shape::Circle) {
			// place vertices of the boundary on a circle of radius 1,
			// starting at (1,0), in counterclockwise order

			// angle increment
			float inc = (2.0f*M_PI)/boundary.size();
			float alpha = 0.0f;
			for (size_t i = 0; i < boundary.size(); ++i, alpha += inc) {
				int v_idx = m.get_vertex_corner(boundary[i]);
				uvs[v_idx].x = std::cos(alpha);
				uvs[v_idx].y = std::sin(alpha);
			}
		}
		else if (s == boundary_shape::Square) {
			// place vertices of the boundary on a square of side
			// length 1, starting at (1,-1), in counterclockwise order
			int r = boundary.size()%4;
			int n_side1 = boundary.size()/4 + (r > 0 ? 1 : 0); --r;
			int n_side2 = boundary.size()/4 + (r > 0 ? 1 : 0); --r;
			int n_side3 = boundary.size()/4 + (r > 0 ? 1 : 0); --r;
			int n_side4 = boundary.size()/4 + (r > 0 ? 1 : 0);

			// texture coordinates of boundary vertices
			size_t bound_it = 0;
			for (int i = 0; i < n_side1; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				uvs[v_idx].x = 1.0f;
				uvs[v_idx].y = -1.0f + i*(2.0f/n_side1);
			}
			for (int i = 0; i < n_side2; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				uvs[v_idx].x = 1.0f - i*(2.0f/n_side2);
				uvs[v_idx].y = 1.0f;
			}
			for (int i = 0; i < n_side3; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				uvs[v_idx].x = -1.0f;
				uvs[v_idx].y = 1.0f - i*(2.0f/n_side3);
			}
			for (int i = 0; i < n_side4; ++i, ++bound_it) {
				int v_idx = m.get_vertex_corner(boundary[bound_it]);
				uvs[v_idx].x = -1.0f + i*(2.0f/n_side4);
				uvs[v_idx].y = -1.0f;
			}
		}




		return true;
	}

} // -- namespace parametrisation
} // -- namespace geoproc
