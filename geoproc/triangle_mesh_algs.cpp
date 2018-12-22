#include <geoproc/triangle_mesh.hpp>

// C includes
#include <assert.h>

// C++ includes
#include <functional>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <set>
using namespace std;

struct CornerEdgeFace {
	// vA, vB: vertex indices
	int vA, vB;
	// corner index (basically, tri_index/3)
	int corner;
	// (normalised) triangle index
	int tri_index;

	// Were the indices sorted in the set_indices
	// method? We need this to work out the left
	// and right triangles.
	bool sorted;

	CornerEdgeFace() : vA(-1), vB(-1), corner(-1), tri_index(-1), sorted(false) { }
	CornerEdgeFace(const CornerEdgeFace& cef) {
		vA = cef.vA;
		vB = cef.vB;
		corner = cef.corner;
		tri_index = cef.tri_index;
		sorted = cef.sorted;
	}

	void set_indices(int va, int vb, int c, int t) {
		if (va < vb) {
			vA = va;
			vB = vb;
			sorted = false;
		}
		else {
			vA = vb;
			vB = va;
			sorted = true;
		}
		corner = c;
		tri_index = t;
	}

	bool operator< (const CornerEdgeFace& cef) const {
		return (vA < cef.vA) ||
			  ((vA == cef.vA) && (vB < cef.vB));
	}
	bool operator== (const CornerEdgeFace& cef) const {
		return (vA == cef.vA) && (vB == cef.vB);
	}
};

namespace geoproc {

void TriangleMesh::make_neighbourhood_data() {
	// compute data only when needed
	if (neigh_valid) {
		return;
	}

	// ------------------
	// build corner table
	corners.resize(vertices.size());

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		// triangle i has 3 corners:
		// -> i+0, i+1, i+2
		// each corner j is associated to a vertex
		// triangles[i + 1] = j <-> "the i+1 corner of the
		// triangle has vertex j".

		int v0 = triangles[i];
		corners[v0] = i;

		int v1 = triangles[i + 1];
		corners[v1] = i + 1;

		int v2 = triangles[i + 2];
		corners[v2] = i + 2;
	}

	// ----------------------------
	// build opposite_corners table

	// opposite_corners is initialised with -1 because
	// such opposite may not exist.
	opposite_corners.resize(triangles.size(), -1);

	/* build data structure so that we have pairs of edges
	 * (an edge is a pair of vertex indices) each associated
	 * to a corner index:
	 * if we have, for an edge (i,j)
	 *     i j c
	 *     i j o
	 * then it means that corner 'c' has opposite 'o',
	 * and viceversa. Therefore:
	 *     opposite_cornes[o] = c
	 *     opposite_cornes[c] = o
	 */
	std::vector<CornerEdgeFace> corner_data;
	for (int i = 0; i < n_corners(); i += 3) {
		int c0 = i;		// corner index
		int c1 = i + 1;	// corner index
		int c2 = i + 2;	// corner index

		CornerEdgeFace e1, e2, e3;

		// edge v0,v1  (vertex indices)
		e1.set_indices(triangles[c0], triangles[c1], c2, i/3);
		// edge v1,v2  (vertex indices)
		e2.set_indices(triangles[c1], triangles[c2], c0, i/3);
		// edge v0,v2  (vertex indices)
		e3.set_indices(triangles[c0], triangles[c2], c1, i/3);

		corner_data.push_back(e1);
		corner_data.push_back(e2);
		corner_data.push_back(e3);
	}
	// sort the vector so that the edge pairs are consecutive
	std::sort(corner_data.begin(), corner_data.end());

	// helper for filling missing opposites
	set<int> boundary_vertices;

	all_edges.clear();
	boundary_edges.clear();

	// For each corner edge, check that a twin exists.
	// If so, we have found a pair of opposite corners.
	// Notice that, by construction*, twins are consecutive.
	// *: corner_data is sorted!
	for (size_t i = 0; i < corner_data.size(); ++i) {
		// take vertex indices
		int vA = corner_data[i].vA;
		int vB = corner_data[i].vB;

		bool has_twin = false;
		if (i + 1 < corner_data.size()) {
			const CornerEdgeFace& cef = corner_data[i + 1];
			if (cef.vA == vA and cef.vB == vB) {
				// a twin exists
				int o = corner_data[i].corner;
				int c = corner_data[i + 1].corner;
				opposite_corners[o] = c;
				opposite_corners[c] = o;

				has_twin = true;
				// SKIP (go to the next pair)
				++i;
			}
		}

		if (not has_twin) {
			// this single CornerEdgeFace contains
			// an edge of the boundary -> hard boundary

			// obtain next and previous corners of data[i].corner
			int corner = corner_data[i].corner;
			int next_corner = 3*(corner/3) + (corner + 1)%3;
			int prev_corner = 3*(corner/3) + (corner + 2)%3;

			// At this point the left and right triangles are
			// wrong! They are corrected in the coming code
			boundary_edges.push_back(
				MeshEdge(get_vertex_corner(next_corner),
						 get_vertex_corner(prev_corner),
						 -1, -1)
			);

			boundary_vertices.insert(vA);
			boundary_vertices.insert(vB);
		}
	}

	// -------------------
	// Build edges of mesh

	vertex_edge.resize(vertices.size());
	// we have to use the fact that corner_data is sorted.
	for (size_t i = 0; i < corner_data.size(); ++i) {

		// vertex indices
		int vA = corner_data[i].vA;
		int vB = corner_data[i].vB;

		MeshEdge E;
		E.v0 = get_vertex_corner(vA);
		E.v1 = get_vertex_corner(vB);
		if (corner_data[i].sorted) {
			E.left_trgl = corner_data[i].tri_index;
		}
		else {
			E.right_trgl = corner_data[i].tri_index;
		}

		if (i + 1 < corner_data.size()) {
			const CornerEdgeFace& cef = corner_data[i + 1];
			if (cef.vA == vA and cef.vB == vB) {
				++i;
				// if a twin exists then we can find
				// the missing adjacent triangle
				if (cef.sorted) {
					E.left_trgl = cef.tri_index;
				}
				else {
					E.right_trgl = cef.tri_index;
				}
			}
		}

		// If it does not have a twin then the edge will
		// have a -1 at some adjacent triangle index

		// whether it has a twin or not, the edge
		// will not be inserted twice
		all_edges.push_back(E);

		// relate this edge to its first vertex for fast
		// retrieval (given a vertex, return an edge index)
		vertex_edge[E.v0] = all_edges.size() - 1;
	}

	// because we have done too many push_backs
	all_edges.shrink_to_fit();

	#if defined (DEBUG)
	// -------------------------------------
	// sanity check (debug compilation only)

	bool sane = true;
	size_t i = 0;
	while (i < opposite_corners.size() and sane) {
		if (opposite_corners[i] != -1) {
			int o = opposite_corners[i];
			int c = opposite_corners[o];
			/* we must have
			 *     opposite_corners[c] = o <-> opposite_corners[o] = c
			 */
			if ((int)(i) != c) {
				cerr << "         corner " << i << " has opposite " << o << endl;
				cerr << "however, corner " << o << " has opposite " << c << endl;
				sane = false;
			}
		}
		++i;
	}
	assert( sane );
	#endif

	// -------------------------
	// Fill in missing opposites

	// Now that we know the boundary edges it
	// is time to compute the missing opposites

	// Try to iterate in counterclockwise order and
	// in clockwise order the one-ring neighbourhood
	// of all boundary vertices

	auto next_corner = [](int c) -> int {return 3*(c/3) + (c + 1)%3;};
	auto prev_corner = [](int c) -> int {return 3*(c/3) + (c + 2)%3;};

	// temporary vector to find opposites
	vector<int> temp_opposite_corners(triangles.size(), -1);

	for (int bv : boundary_vertices) {
		int c = corners[bv];
		bool terminate = false;

		int last_counterclockwise;
		int last_clockwise;

		// iterate in counterclockwise order
		int cc = c;
		while (not terminate) {
			int ncc = next_corner(cc);
			int o = opposite_corners[ncc];
			if (o == -1) {
				last_counterclockwise = ncc;
				terminate = true;
			}
			else {
				cc = next_corner(o);
			}
		}

		// iterate in clockwise order
		terminate = false;
		while (not terminate) {
			int ncc = prev_corner(cc);
			int o = opposite_corners[ncc];
			if (o == -1) {
				last_clockwise = ncc;
				terminate = true;
			}
			else {
				cc = prev_corner(o);
			}
		}

		// store the opposite information before
		// finishing the whole algorithm
		temp_opposite_corners[ last_counterclockwise ] = last_clockwise;
	}

	// store missing opposites
	for (size_t i = 0; i < triangles.size(); ++i) {
		if (temp_opposite_corners[i] != -1) {
			opposite_corners[i] = -temp_opposite_corners[i];
		}
	}

	// validate neighbourhood data
	neigh_valid = true;
	// boundaries are not valid
	invalidate_boundaries();
}

void TriangleMesh::make_boundaries() {
	assert(neigh_valid);

	if (boundaries_valid) {
		return;
	}

	// vector to avoid redundant computations
	vector<bool> visited(boundary_edges.size(), false);
	// for each boundary edge in the mesh...
	for (size_t i = 0; i < boundary_edges.size(); ++i) {
		if (visited[i]) { continue; }

		const MeshEdge& ei = boundary_edges[i];

		// find the boundary starting at i-th edge
		visited[i] = true;
		boundaries.push_back(vector<int>());
		boundaries.back().push_back(ei.v0);

		size_t j;
		int next_vertex = ei.v1;
		do {
			// find a boundary edge that shares
			// a vertex with i-th edge. Let i-th
			// edge be (u,v)

			// add_corner is the corner that is not shared
			// with i-th edge: if j-th edge is (u,x) then
			//		add_corner = corner of x

			int add_vertex = -1;
			bool found = false;
			j = i + 1;
			while (not found and j < boundary_edges.size()) {
				if (visited[j]) { ++j; continue; }

				// corner and vertex pair
				const MeshEdge& ej = boundary_edges[j];

				if (ej.v0 == next_vertex) {
					add_vertex = next_vertex;
					next_vertex = ej.v1;
					visited[j] = true;
					found = true;
				}
				else if (ej.v1 == next_vertex) {
					add_vertex = next_vertex;
					next_vertex = ej.v0;
					visited[j] = true;
					found = true;
				}
				++j;
			}

			if (add_vertex == -1) {
				if (ei.v0 == next_vertex) {
					add_vertex = next_vertex;
					next_vertex = ei.v1;
				}
				else if (ei.v1 == next_vertex) {
					add_vertex = next_vertex;
					next_vertex = ei.v0;
				}
			}

			// if add_vertex is -1 then it means that we
			// could not find another edge to continue the
			// boundary. Therefore, we can't close it.
			assert(add_vertex != -1);

			boundaries.back().push_back(add_vertex);
		}
		while (next_vertex != ei.v0);
	}

	boundaries_valid = true;
}

} // -- namespace geoproc
