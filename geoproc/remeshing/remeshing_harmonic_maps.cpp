#include <geoproc/remeshing/remeshing.hpp>

// C++ includes
#include <iostream>
#include <vector>
using namespace std;

// glm includes
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/mesh_edge.hpp>

#define triangle_orientation(p1,p2,p3)								\
	((p1.x - p3.x)*(p2.y - p3.y) - (p1.y - p3.y)*(p2.x - p3.x))

// i ranges from 0 to n-1
// j ranges from 0 to m-1
#define index_point(i,j, n) ((j)*n + (i))

#define vec2out(p) "(" << p.x << "," << p.y << ")"

enum seg_ori {
	collinear,
	left, right
};

inline bool inside_triangle
(const vector<vec2>& uvs, const vec2& p, int v0, int v1, int v2)
{
	const vec2& p0 = uvs[v0];
	const vec2& p1 = uvs[v1];
	const vec2& p2 = uvs[v2];

	cout << "        Triangle " << vec2out(p0) << ", "
							<< vec2out(p1) << ", "
							<< vec2out(p2) << endl;

	// triangle orientations
	float tri_ori = triangle_orientation(p0, p1, p2);
	if (std::abs(tri_ori) <= 1e-6f) {
		// degenerate triangle. We don't want it or we'll have
		// division by 0 when computing barycentric coordinates
		return false;
	}

	float o0 = triangle_orientation(p0,p1,p);
	float o1 = triangle_orientation(p1,p2,p);
	float o2 = triangle_orientation(p2,p0,p);

	cout << "        tri_ori= " << tri_ori << endl;
	cout << "        o0= " << o0 << endl;
	cout << "        o1= " << o1 << endl;
	cout << "        o2= " << o2 << endl;

	if (tri_ori >= 0.0f) {
		return o0 >= 0.0f and o1 >= 0.0f and o2 >= 0.0f;
	}
	return o0 <= 0.0f and o1 <= 0.0f and o2 <= 0.0f;
}

inline float triangle_area
(const vec2& p, const vec2& q, const vec2& r)
{
	vec3 p3(p, 0.0f);
	vec3 q3(q, 0.0f);
	vec3 r3(r, 0.0f);

	vec3 ij = q3 - p3;
	vec3 ik = r3 - p3;
	vec3 C = cross(ij, ik);
	return length(C)/2.0f;
}

inline seg_ori segment_orientation
(const vec2& p, const vec2& q, const vec2& r)
{
	float v = (q.x - p.x)*(r.y - p.y) - (q.y - p.y)*(r.x - p.x);
	cout << "        v= " << v;
	if (std::abs(v) <= 1.0e-20f) {
		cout << " -> collinear" << endl;
		return seg_ori::collinear;
	}
	if (v < 0.0f) {
		cout << " -> right" << endl;
		return seg_ori::right;
	}

	cout << " -> left" << endl;
	return seg_ori::left;
}

// returns true if segment (p,q) intersects segment (r,s)
inline bool get_next_triangle(
	const vector<vec2>& uvs, const vector<geoproc::mesh_edge>& dcel,
	int edge_idx, const vec2& p, const vec2& q
)
{
	// get edges and vertices of current triangle
	int v0 = dcel[edge_idx].v0;
	int v1 = dcel[edge_idx].v1;
	const vec2& r = uvs[v0];
	const vec2& s = uvs[v1];

	cout << "    edge " << edge_idx << ": " << v0 << " -> " << v1 << endl;
	cout << "        " << vec2out(r) << " -> " << vec2out(s) << endl;

	seg_ori pq_r = segment_orientation(p,q, r);
	seg_ori pq_s = segment_orientation(p,q, s);
	seg_ori rs_p = segment_orientation(r,s, p);
	seg_ori rs_q = segment_orientation(r,s, q);

	cout << "        orientation(p,q, r): "
		 << (pq_r == seg_ori::collinear ? "collinear" : pq_r == seg_ori::left ? "left" : "right")
		 << endl;
	cout << "        orientation(p,q, s): "
		 << (pq_s == seg_ori::collinear ? "collinear" : pq_s == seg_ori::left ? "left" : "right")
		 << endl;
	cout << "        orientation(r,s, p): "
		 << (rs_p == seg_ori::collinear ? "collinear" : rs_p == seg_ori::left ? "left" : "right")
		 << endl;
	cout << "        orientation(r,s, q): "
		 << (rs_q == seg_ori::collinear ? "collinear" : rs_q == seg_ori::left ? "left" : "right")
		 << endl;

	// general case: intersection between the vertices
	if (pq_r != pq_s and rs_p != rs_q) {
		cout << "        ++ crossing" << endl;
		return true;
	}

#define lt(i, j) ((i.x <= j.x) and (i.y <= j.y))
#define on_segment(i,j, k) ((lt(i,k) and lt(k,j)) or (lt(j,k) and lt(k,i)))

	// degenerate cases

	// p,q, r are colinear and r lies on segment pq
	if (pq_r == seg_ori::collinear and on_segment(p,q, r)) {
		cout << "        ++ collinearity" << endl;
		return true;
	}
	// p,q, s are colinear and s lies on segment pq
	if (pq_s == seg_ori::collinear and on_segment(p,q, s)) {
		cout << "        ++ collinearity" << endl;
		return true;
	}

	// r,s, p are colinear and p lies on segment rs
	if (rs_p == seg_ori::collinear and on_segment(r,s, p)) {
		cout << "        ++ collinearity" << endl;
		return true;
	}

	 // r,s, q are colinear and q lies on segment rs
	if (rs_q == seg_ori::collinear and on_segment(r,s, q)) {
		cout << "        ++ collinearity" << endl;
		return true;
	}

	// no intersection
	cout << "        .. no intersection" << endl;
	return false;
}

// linear search through all the meshe's triangles
inline int find_first_triangle
(const geoproc::TriangleMesh& mesh, const vector<vec2>& uvs, const vec2& p)
{
	for (int t = 0; t < mesh.n_triangles(); ++t) {
		int v0,v1,v2;
		mesh.get_vertices_triangle(t, v0,v1,v2);

		cout << "    Checking triangle " << t << endl;
		cout << "        with vertices " << v0 << "," << v1 << "," << v2 << endl;
		cout << "        Is point (" << p.x << "," << p.y << ") inside this triangle?" << endl;

		if (inside_triangle(uvs, p, v0,v1,v2)) {
			cout << "        --> yes" << endl;
			return t;
		}
		cout << "        --> no" << endl;
	}

	// no triangle found
	return -1;
}

// locate the triangle where 'next' is in using the fact
// that point 'pre' is located in triangle 'pre_tri'
inline int find_next_triangle(
	const geoproc::TriangleMesh& mesh, int prev_tri,
	const vector<vec2>& uvs, const vec2& prev, const vec2& next
)
{
	const vector<geoproc::mesh_edge>& dcel = mesh.get_edges();
	const vector<int[3]>& edges_per_triangle = mesh.get_edges_triangle();

	// assume we have a segment from 'pre' to 'next'

	cout << "    Finding next triangle" << endl;
	cout << "        for next point: " << vec2out(next) << endl;
	cout << "        from previous point: " << vec2out(prev) << endl;
	cout << "            in triangle: " << prev_tri << endl;

	int T = prev_tri;
	bool finish = false;
	// prev_edge to avoid infinite loop going back and forth
	int prev_edge = -1;

	while (not finish) {
		int vt0, vt1, vt2;
		mesh.get_vertices_triangle(T, vt0,vt1,vt2);
		const int *ept = edges_per_triangle[T];

		cout << "    Checking triangle " << T << endl;
		cout << "        with vertices " << vt0 << "," << vt1 << "," << vt2 << endl;
		cout << "        and edges " << ept[0] << "," << ept[1] << "," << ept[2] << endl;
		cout << "        Is point (" << next.x << "," << next.y << ") inside this triangle?" << endl;

		if (inside_triangle(uvs, next, vt0,vt1,vt2)) {
			finish = true;
			cout << "        --> yes. Finish" << endl;
			continue;
		}
		cout << "        --> no. Continue" << endl;

		int e_idx;

		bool next_found = false;
		e_idx = ept[0];
		if (not next_found and (prev_edge == -1 or prev_edge != e_idx)) {
			next_found = get_next_triangle(uvs, dcel, e_idx, prev, next);
			if (next_found) {
				T = (dcel[e_idx].lT == T ? dcel[e_idx].rT : dcel[e_idx].lT);
				prev_edge = e_idx;
			}
		}

		e_idx = ept[1];
		if (not next_found and (prev_edge == -1 or prev_edge != e_idx)) {
			next_found = get_next_triangle(uvs, dcel, e_idx, prev, next);
			if (next_found) {
				T = (dcel[e_idx].lT == T ? dcel[e_idx].rT : dcel[e_idx].lT);
				prev_edge = e_idx;
			}
		}

		e_idx = ept[2];
		if (not next_found and (prev_edge == -1 or prev_edge != e_idx)) {
			next_found = get_next_triangle(uvs, dcel, e_idx, prev, next);
			if (next_found) {
				T = (dcel[e_idx].lT == T ? dcel[e_idx].rT : dcel[e_idx].lT);
				prev_edge = e_idx;
			}
		}

		if (not next_found) {
			cerr << "    Error in" << endl;
			cerr << "        file: remeshing_harmonic_maps.cpp" << endl;
			cerr << "        line: " << __LINE__ << endl;
			cerr << "        When trying to locate point ("
				 << next.x << "," << next.y << ")" << endl;
			cerr << "        Triangle " << T << " does not intersect segment" << endl;
			cerr << "        (" << prev.x << "," << prev.y << ") -> "
				 << "(" << next.x << "," << next.y << ")" << endl;

			T = -1;
			finish = true;
		}

		cout << "        next triangle to look at: " << T << endl;
	}
	return T;
}

inline void barycentric_coordinates(
	const geoproc::TriangleMesh& mesh, int T,
	const vector<vec2>& uvs, const vec2& p,
	float& w0, float& w1, float& w2
)
{
	float aT = mesh.get_triangle_area(T);
	int v0, v1, v2;
	mesh.get_vertices_triangle(T, v0,v1,v2);

	float a0 = triangle_area(p, uvs[v1], uvs[v2]);
	float a1 = triangle_area(uvs[v0], p, uvs[v2]);
	float a2 = triangle_area(uvs[v0], uvs[v1], p);

	w0 = a0/aT;
	w1 = a1/aT;
	w2 = a2/aT;
}

inline void make_new_vertex
(const geoproc::TriangleMesh& mesh, int T, float w0,float w1,float w2, vec3& vert)
{
	int v0, v1, v2;
	mesh.get_vertices_triangle(T, v0,v1,v2);

	const vector<vec3>& vertices = mesh.get_vertices();
	vert = vertices[v0]*w0 + vertices[v1]*w1 + vertices[v2]*w2;
}

#define it_start_at 1
#define it_end_at(k) (k - 1) // included end
#define point_start_at(k) ((1.0f*it_start_at)/k)

namespace geoproc {
using namespace parametrisation;
using namespace smoothing;

namespace remeshing {

	bool harmonic_maps(
		const TriangleMesh& mesh, size_t N, size_t M, const smooth_weight& w,
		const boundary_shape& s, TriangleMesh& remesh
	)
	{
		if (s == boundary_shape::Circle) {
			cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
			cerr << "    Not implemented for shape 'Circle'." << endl;
			return false;
		}

		vector<vec2> uvs;
		bool r = parametrisation::harmonic_maps(mesh, w, s, uvs);
		if (not r) {
			return false;
		}

		size_t it = 0;
		vec2 pre(point_start_at(N), point_start_at(M));
		vector<vec3> new_vertices(N*M);
		float w0,w1,w2;

		/* compute the coordinates of the new vertices */

		cout << "Locating point " << vec2out(pre) << endl;

		int nT = find_first_triangle(mesh, uvs, pre);
		if (nT == -1) {
			cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
			cerr << "    Could not locate point " << vec2out(pre) << "." << endl;
			return false;
		}

		barycentric_coordinates(mesh, nT, uvs, pre, w0,w1,w2);
		make_new_vertex(mesh, nT, w0,w1,w2, new_vertices[it]);
		++it;

		// first row of points
		for (size_t j = it_start_at + 1; j <= it_end_at(M); ++j) {
			vec2 next(point_start_at(N), (1.0f*j)/M);
			cout << "Locating point " << vec2out(next)
				 << " (index: " << it_start_at << "," << j << ")" << endl;

			nT = find_next_triangle(mesh, nT, uvs, pre, next);
			if (nT == -1) {
				cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
				cerr << "    Could not locate point (0," << j << ")= ("
					 << next.x << "," << next.y << ")" << endl;
				return false;
			}

			cout << "    ** Point " << vec2out(next) << " is in triangle " << nT << endl;

			barycentric_coordinates(mesh, nT, uvs, pre, w0,w1,w2);

			cout << "    weights: w0= " << w0 << endl;
			cout << "             w1= " << w1 << endl;
			cout << "             w2= " << w2 << endl;

			make_new_vertex(mesh, nT, w0,w1,w2, new_vertices[it]);
			++it;

			pre = next;
		}

		// rest of the grid
		for (size_t i = it_start_at + 1; i <= it_end_at(N); ++i) {
			for (size_t j = it_start_at; j <= it_end_at(M); ++j) {
				vec2 next((1.0f*i)/N, (1.0f*j)/M);

				cout << "Locating point " << vec2out(next)
					 << " (index: " << i << "," << j << ")" << endl;

				nT = find_next_triangle(mesh, nT, uvs, pre, next);
				if (nT == -1) {
					cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
					cerr << "    Could not locate point (" << i << "," << j << ")= ("
						 << next.x << "," << next.y << ")" << endl;
					return false;
				}

				cout << "    ** Point " << vec2out(next) << " is in triangle " << nT << endl;

				barycentric_coordinates(mesh, nT, uvs, pre, w0,w1,w2);

				cout << "    weights: w0= " << w0 << endl;
				cout << "             w1= " << w1 << endl;
				cout << "             w2= " << w2 << endl;

				make_new_vertex(mesh, nT, w0,w1,w2, new_vertices[it]);
				++it;

				pre = next;
			}
		}

		/* compute new triangles */
		// this is easy because we handle only the 'Square' case
		vector<int> new_triangles;
		for (size_t i = it_start_at; i <= it_end_at(N); ++i) {
			for (size_t j = it_start_at; j <= it_end_at(M); ++j) {
				new_triangles.push_back(index_point(i,j, N));
				new_triangles.push_back(index_point(i+1,j, N));
				new_triangles.push_back(index_point(i+1,j+1, N));

				new_triangles.push_back(index_point(i,j, N));
				new_triangles.push_back(index_point(i+1,j+1, N));
				new_triangles.push_back(index_point(i,j+1, N));
			}
		}

		remesh.set_vertices(new_vertices);
		remesh.set_triangles(new_triangles);
		return true;
	}

} // -- namespace remeshing
} // -- namespace geoproc
