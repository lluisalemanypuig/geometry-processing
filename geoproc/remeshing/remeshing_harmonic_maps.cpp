#include <geoproc/remeshing/remeshing.hpp>

// C includes
#include <omp.h>

// C++ includes
#include <iostream>
#include <vector>
using namespace std;

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/definitions.hpp>
#include <geoproc/mesh_edge.hpp>

// helper defines
#define triangle_orientation(p1,p2,p3) ((p1.x - p3.x)*(p2.y - p3.y) - (p1.y - p3.y)*(p2.x - p3.x))
#define vec2out(p) "Point({" << p.x << "," << p.y << "})"
#define segment2out(p,q) "Segment(" << vec2out(p) << "," << vec2out(q) << ")"

/* orientation and intersection tests */
enum seg_ori {
	collinear,
	left, right
};

inline bool inside_triangle
(const vector<vec2d>& uvs, const vec2d& p, int v0, int v1, int v2)
{
	const vec2d& p0 = uvs[v0];
	const vec2d& p1 = uvs[v1];
	const vec2d& p2 = uvs[v2];
	// triangle orientations
	double tri_ori = triangle_orientation(p0, p1, p2);
	double o0 = triangle_orientation(p0,p1,p);
	double o1 = triangle_orientation(p1,p2,p);
	double o2 = triangle_orientation(p2,p0,p);
	if (tri_ori >= 0.0) {
		return o0 >= 0.0 and o1 >= 0.0 and o2 >= 0.0;
	}
	return o0 <= 0.0 and o1 <= 0.0 and o2 <= 0.0;
}

inline double triangle_area
(const vec2d& p, const vec2d& q, const vec2d& r)
{
	vec3d p3(p, 0.0);
	vec3d q3(q, 0.0);
	vec3d r3(r, 0.0);

	vec3d ij = q3 - p3;
	vec3d ik = r3 - p3;
	vec3d C = cross(ij, ik);
	return length(C)/2.0;
}

inline seg_ori segment_orientation
(const vec2d& p, const vec2d& q, const vec2d& r)
{
	double v = (q.x - p.x)*(r.y - p.y) - (q.y - p.y)*(r.x - p.x);
	if (std::abs(v) <= 1.0e-20) {
		return seg_ori::collinear;
	}
	if (v < 0.0) {
		return seg_ori::right;
	}
	return seg_ori::left;
}

// returns true if segment (p,q) intersects segment (r,s)
inline bool segment_intersection(
	const vector<vec2d>& uvs, const vector<geoproc::mesh_edge>& dcel,
	int edge_idx, const vec2d& p, const vec2d& q
)
{
	// get edges and vertices of current triangle
	int v0 = dcel[edge_idx].v0;
	int v1 = dcel[edge_idx].v1;
	const vec2d& r = uvs[v0];
	const vec2d& s = uvs[v1];

	seg_ori pq_r = segment_orientation(p,q, r);
	seg_ori pq_s = segment_orientation(p,q, s);
	seg_ori rs_p = segment_orientation(r,s, p);
	seg_ori rs_q = segment_orientation(r,s, q);

	// general case: intersection between the vertices
	if (pq_r != pq_s and rs_p != rs_q) {
		return true;
	}

#define lt(i, j) ((i.x <= j.x) and (i.y <= j.y))
#define on_segment(i,j, k) ((lt(i,k) and lt(k,j)) or (lt(j,k) and lt(k,i)))

	// degenerate cases, intersection happens because one
	// of the vertices of one segment is on the other segment

	// p,q, r are colinear and r lies on segment pq
	if (pq_r == seg_ori::collinear and on_segment(p,q, r)) {
		return true;
	}
	// p,q, s are colinear and s lies on segment pq
	if (pq_s == seg_ori::collinear and on_segment(p,q, s)) {
		return true;
	}

	// r,s, p are colinear and p lies on segment rs
	if (rs_p == seg_ori::collinear and on_segment(r,s, p)) {
		return true;
	}
	// r,s, q are colinear and q lies on segment rs
	if (rs_q == seg_ori::collinear and on_segment(r,s, q)) {
		return true;
	}

	// no intersection
	return false;
}

/* point-location functions */

// linear search through all the meshe's triangles
inline int find_first_triangle
(const geoproc::TriangleMesh& mesh, const vector<vec2d>& uvs, const vec2d& p)
{
	for (int t = 0; t < mesh.n_triangles(); ++t) {
		int v0,v1,v2;
		mesh.get_vertices_triangle(t, v0,v1,v2);
		if (inside_triangle(uvs, p, v0,v1,v2)) {
			return t;
		}
	}

	// no triangle found
	return -1;
}

// locate the triangle where 'next' is in using the fact
// that point 'pre' is located in triangle 'pre_tri'
inline int find_next_triangle(
	const geoproc::TriangleMesh& mesh, int prev_tri,
	const vector<vec2d>& uvs, const vec2d& prev, const vec2d& next
)
{
	const vector<geoproc::mesh_edge>& dcel = mesh.get_edges();
	const vector<vec3i>& edges_per_triangle = mesh.get_edges_triangle();

	// assume we have a segment from 'pre' to 'next'
	int T = prev_tri;
	bool finish = false;
	// prev_edge to avoid infinite loop going back and forth
	int prev_edge = -1;

	//cout << "Trying to locate point " << vec2out(next) << endl;

	while (not finish) {
		int vt0, vt1, vt2;
		mesh.get_vertices_triangle(T, vt0,vt1,vt2);
		const vec3i& ept = edges_per_triangle[T];

		//cout << "    Checking triangle " << T << endl;
		//cout << "    Vertices of triangle:" << endl;
		//cout << "        " << vt0 << ": " << vec2out(uvs[vt0]) << endl;
		//cout << "        " << vt1 << ": " << vec2out(uvs[vt1]) << endl;
		//cout << "        " << vt2 << ": " << vec2out(uvs[vt2]) << endl;

		//cout << "    Is point " << vec2out(next) << " in triangle?" << endl;
		if (inside_triangle(uvs, next, vt0,vt1,vt2)) {
			//cout << "    Yes" << endl;
			finish = true;
			continue;
		}
		int e_idx;

		//cout << "        No. Previous edge: " << prev_edge << endl;
		//cout << "        Segment: " << segment2out(prev, next) << endl;

		bool seg_int = false;

		e_idx = ept[0];
		//cout << "        Check segment intersection with edge " << ept[0] << "?" << endl;
		if (not seg_int and (prev_edge == -1 or prev_edge != e_idx)) {
			seg_int = segment_intersection(uvs, dcel, e_idx, prev, next);
			//cout << "            Does segment intersect edge?" << endl;
			if (seg_int) {
				//cout << "            Yes" << endl;
				T = (dcel[e_idx].lT == T ? dcel[e_idx].rT : dcel[e_idx].lT);
				prev_edge = e_idx;
			}
			else {
			}
		}

		e_idx = ept[1];
		//cout << "        Check segment intersection with edge " << ept[1] << "?" << endl;
		if (not seg_int and (prev_edge == -1 or prev_edge != e_idx)) {
			seg_int = segment_intersection(uvs, dcel, e_idx, prev, next);
			//cout << "            Does segment intersect edge?" << endl;
			if (seg_int) {
				//cout << "            Yes" << endl;
				T = (dcel[e_idx].lT == T ? dcel[e_idx].rT : dcel[e_idx].lT);
				prev_edge = e_idx;
			}
		}

		e_idx = ept[2];
		//cout << "        Check segment intersection with edge " << ept[2] << "?" << endl;
		if (not seg_int and (prev_edge == -1 or prev_edge != e_idx)) {
			seg_int = segment_intersection(uvs, dcel, e_idx, prev, next);
			//cout << "            Does segment intersect edge?" << endl;
			if (seg_int) {
				//cout << "            Yes" << endl;
				T = (dcel[e_idx].lT == T ? dcel[e_idx].rT : dcel[e_idx].lT);
				prev_edge = e_idx;
			}
		}

		if (not seg_int) {
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
	}
	return T;
}

inline void make_new_vertex(
	const geoproc::TriangleMesh& mesh, const vector<vec2d>& uvs,
	int T, const vec2d& p, vec3d& vert
)
{
	int v0, v1, v2;
	mesh.get_vertices_triangle(T, v0,v1,v2);

	double aT = triangle_area(uvs[v0], uvs[v1], uvs[v2]);
	double a0 = triangle_area(p, uvs[v1], uvs[v2]);
	double a1 = triangle_area(uvs[v0], p, uvs[v2]);
	double a2 = triangle_area(uvs[v0], uvs[v1], p);

	double w0 = a0/aT;
	double w1 = a1/aT;
	double w2 = a2/aT;

	// this check makes sure that the point
	// is actually inside triangle 'T'
	assert(std::abs(w0 + w1 + w2 - 1.0) <= 1e-6);

	const vector<vec3d>& vertices = mesh.get_vertices();
	vert = vertices[v0]*w0 + vertices[v1]*w1 + vertices[v2]*w2;
}

// make vertices from grid point with index 'from' to
// grid point with index 'to'. Interval [from, to).
inline bool make_vertices_from_to(
	const geoproc::TriangleMesh& mesh,
	const vector<vec2d>& uvs,
	size_t N, size_t M, size_t begin, size_t end,
	vector<vec3d>& new_vertices
)
{
	double x_idx = (begin/M)*1.0;
	double y_idx = (begin%M)*1.0;
	vec2d pre((x_idx + 1)/(N + 1), (y_idx + 1)/(M + 1));

	// first point - to be handled differently
	int nT = find_first_triangle(mesh, uvs, pre);
	if (nT == -1) {
		cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
		cerr << "    In thread: " << omp_get_thread_num() << endl;
		cerr << "    Could not locate point ("
			 << x_idx << "," << y_idx << ")= " << vec2out(pre) << endl;
		return false;
	}
	make_new_vertex(mesh, uvs, nT, pre, new_vertices[begin]);

	//cout << "Point (" << begin << "): " << vec2out(pre)
	//	 << " in triangle " << nT << endl;

	// rest of the points from 'begin' to 'end'
	for (size_t idx = begin + 1; idx < end; ++idx) {
		x_idx = (idx/M)*1.0;
		y_idx = (idx%M)*1.0;
		vec2d next((x_idx + 1)/(N + 1), (y_idx + 1)/(M + 1));

		nT = find_next_triangle(mesh, nT, uvs, pre, next);
		if (nT == -1) {
			cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
			cerr << "    In thread: " << omp_get_thread_num() << endl;
			cerr << "    Could not locate point ("
				 << x_idx << "," << y_idx << ")= " << vec2out(next) << endl;
			return false;
		}
		//cout << "Point (" << idx << "): " << vec2out(next)
		//	 << " in triangle " << nT << endl;

		make_new_vertex(mesh, uvs, nT, next, new_vertices[idx]);
		pre = next;
	}

	return true;
}

namespace geoproc {
using namespace parametrisation;
namespace remeshing {

bool harmonic_maps(
	const TriangleMesh& mesh, size_t N, size_t M, const weight& w,
	const boundary_shape& s, TriangleMesh& remesh
)
{
	if (s == boundary_shape::Circle) {
		cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
		cerr << "    Not implemented for shape 'Circle'." << endl;
		return false;
	}

	vector<vec2d> uvs;
	bool r = parametrisation::harmonic_maps(mesh, w, s, uvs);
	if (not r) {
		return false;
	}

	return harmonic_maps(mesh, N, M, uvs, remesh);
}

bool harmonic_maps(
	const TriangleMesh& mesh, size_t N, size_t M,
	const std::vector<glm::vec2d>& uvs, TriangleMesh& remesh
)
{
	assert(mesh.n_vertices() == uvs.size());
	vector<vec3d> new_vertices(N*M);

	/* compute the coordinates of the new vertices */
	bool r = make_vertices_from_to(mesh, uvs, N, M, 0, N*M, new_vertices);
	if (not r) {
		return false;
	}

	/* compute new triangles */
	// this is easy because we handle only the 'Square' case
	vector<int> new_triangles;
	for (size_t i = 0; i < N - 1; ++i) {
		for (size_t j = 0; j < M - 1; ++j) {
			new_triangles.push_back(i*M + j);
			new_triangles.push_back((i + 1)*M + j);
			new_triangles.push_back((i + 1)*M + j + 1);

			new_triangles.push_back(i*M + j);
			new_triangles.push_back((i + 1)*M + j + 1);
			new_triangles.push_back(i*M + j + 1);
		}
	}

	remesh.set_vertices(new_vertices);
	remesh.set_triangles(new_triangles);
	return true;
}

} // -- namespace remeshing
} // -- namespace geoproc
