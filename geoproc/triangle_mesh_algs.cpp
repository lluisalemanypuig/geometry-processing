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

// glm includes
#include <glm/vec3.hpp>
using namespace glm;

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

inline void add_edge_to_triangle
(vector<vec3>& ept, unsigned char *count, int t, int e)
{
	ept[t][ count[t] ] = e;
	++count[t];
}

inline int tri_has_edge_share_vertex
(
	const vector<geoproc::mesh_edge>& edges,
	const vec3& ept, unsigned char count,
	int edge_idx, int endpoint
)
{
	for (unsigned char i = 0; i < count; ++i) {
		if (ept[i] == edge_idx) {
			continue;
		}

		const geoproc::mesh_edge& m_edge = edges[ ept[i] ];
		if (m_edge.v0 == endpoint or m_edge.v1 == endpoint) {
			return ept[i];
		}
	}
	return -1;
}

inline int find_common_boundary_edge(
	const vector<geoproc::mesh_edge>& edges,
	const vector<int>& boundary_edges,
	int edge_idx, int endpoint
)
{
	for (size_t be = 0; be < boundary_edges.size(); ++be) {
		if (boundary_edges[be] == edge_idx) {
			continue;
		}

		int e = boundary_edges[be];
		const geoproc::mesh_edge& m_edge = edges[e];

		if (m_edge.v0 == endpoint or m_edge.v1 == endpoint) {
			return e;
		}
	}
	return -1;
}

inline void find_prev_next_edges(
	const vector<int>& boundary_edges,
	const vector<vec3>& ept, unsigned char *count,
	vector<geoproc::mesh_edge>& edges
)
{
	// for each edge, find the other edges
	// that share the same left/right face
	for (size_t e = 0; e < edges.size(); ++e) {
		geoproc::mesh_edge& cur_edge = edges[e];

		// previous edge: an edge that shares endpoint v0
		// and that also has left triangle lT
		int lT = cur_edge.lT;
		if (lT != -1) {
			// try in interior triangles
			int prev = tri_has_edge_share_vertex(edges, ept[lT], count[lT], e, cur_edge.v0);
			if (prev != -1) {
				cur_edge.pE = prev;
			}
		}
		else {
			// try in boundary triangle
			int prev = find_common_boundary_edge(edges, boundary_edges, e, cur_edge.v0);
			if (prev != -1) {
				cur_edge.pE = prev;
			}
		}

		// next edge: an edge that shares endpoint v1
		// and that also has right triangle rT
		int rT = cur_edge.rT;
		if (rT != -1) {
			// try in left
			int next = tri_has_edge_share_vertex(edges, ept[rT], count[rT], e, cur_edge.v1);
			if (next != -1) {
				cur_edge.nE = next;
			}
		}
		else {
			// try in boundary triangle
			int next = find_common_boundary_edge(edges, boundary_edges, e, cur_edge.v1);
			if (next != -1) {
				cur_edge.nE = next;
			}
		}
	}
}

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
		e3.set_indices(triangles[c2], triangles[c0], c1, i/3);

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
	vertex_edge.clear();

	// -------------------------------
	// Build edges of mesh: build DCEL
	// and find edge boundaries

	vertex_edge.resize(vertices.size(), -1);

	// these are used to find the previous and next edges.
	// edges_per_triangle[t]   : at most three edges such that their
	//                           left/right triangle is triangle 'i'
	edges_per_triangle.resize(n_triangles());
	// count[t] : triangle t has 'count_L/R[t]' edges
	//            such that their left/right triangle
	//            is triangle 'i'
	unsigned char count[n_triangles()];

	// initialise values
	for (int i = 0; i < n_triangles(); ++i) {
		edges_per_triangle[i][0] = -1;
		edges_per_triangle[i][1] = -1;
		edges_per_triangle[i][2] = -1;
		count[i] = 0;
	}

	// For each corner edge, check that a twin exists.
	// If so, we have found a pair of opposite corners.
	// Notice that, by construction*, twins are consecutive.
	// Construct the DCEL while doing so.
	// *: corner_data is sorted!
	for (size_t i = 0; i < corner_data.size(); ++i) {
		const CornerEdgeFace& cef1 = corner_data[i];

		// take vertex indices
		int vA = cef1.vA;
		int vB = cef1.vB;

		mesh_edge E;
		E.v0 = vA;
		E.v1 = vB;
		if (corner_data[i].sorted) {
			E.rT = corner_data[i].tri_index;
		}
		else {
			E.lT = corner_data[i].tri_index;
		}

		bool has_twin = false;
		if (i + 1 < corner_data.size()) {
			const CornerEdgeFace& cef2 = corner_data[i + 1];
			if (cef2.vA == vA and cef2.vB == vB) {
				has_twin = true;
				// if a twin exists then we can find
				// the missing adjacent triangle
				if (cef2.sorted) {
					E.rT = cef2.tri_index;
				}
				else {
					E.lT = cef2.tri_index;
				}

				// make opposites...
				int o = corner_data[i].corner;
				int c = corner_data[i + 1].corner;
				opposite_corners[o] = c;
				opposite_corners[c] = o;

				++i;
			}
		}

		// If it does not have a twin then the edge will
		// have a -1 at some adjacent triangle index

		// whether it has a twin or not, the edge
		// will not be inserted twice
		all_edges.push_back(E);

		int edge_idx = all_edges.size() - 1;
		if (E.lT != -1) {
			add_edge_to_triangle(edges_per_triangle, count, E.lT, edge_idx);
		}
		if (E.rT != -1) {
			add_edge_to_triangle(edges_per_triangle, count, E.rT, edge_idx);
		}

		// relate this edge to its vertices for fast
		// retrieval (given a vertex, return an edge index)
		if (vertex_edge[E.v0] == -1) {
			vertex_edge[E.v0] = edge_idx;
		}
		if (vertex_edge[E.v1] == -1) {
			vertex_edge[E.v1] = edge_idx;
		}

		if (not has_twin) {
			// this single CornerEdgeFace contains
			// an edge of the boundary -> hard boundary

			// At this point the left and right triangles are
			// wrong! They are corrected in the coming code
			boundary_edges.push_back(edge_idx);

			boundary_vertices.insert(vA);
			boundary_vertices.insert(vB);
		}
	}

	// because we have made way too many push_backs
	all_edges.shrink_to_fit();

	// find previous and next edges
	// this is a DCEL
	find_prev_next_edges(boundary_edges, edges_per_triangle, count, all_edges);

	// -------------------------------------
	// sanity check (debug compilation only)
	#if defined (DEBUG)
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

		const mesh_edge& ei = all_edges[boundary_edges[i]];

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
				const mesh_edge& ej = all_edges[boundary_edges[j]];

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
