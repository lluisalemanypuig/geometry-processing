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

#define segment_orientation(p,q, r)									\
	((q.x - p.x)*(r.y - p.y) - (q.y - p.y)*(r.x - p.x))

// i ranges from 0 to n-1
// j ranges from 0 to m-1
#define index_point(i,j, n) ((j)*n + (i))

#define vec2_out(p) "(" << p.x << "," << p.y << ")"

inline bool inside_triangle
(const vector<vec2>& uvs, const vec2& p, int v0, int v1, int v2)
{
	const vec2& p0 = uvs[v0];
	const vec2& p1 = uvs[v1];
	const vec2& p2 = uvs[v2];

	cout << "    Triangle " << vec2_out(p0) << ", "
							<< vec2_out(p1) << ", "
							<< vec2_out(p2) << endl;

	// triangle orientations
	float tri_ori = triangle_orientation(p0, p1, p2);
	if (std::abs(tri_ori) <= 1e-6f) {
		// degenerate triangle
		return false;
	}

	float o0 = triangle_orientation(p0,p1,p);
	float o1 = triangle_orientation(p1,p2,p);
	float o2 = triangle_orientation(p2,p0,p);

	cout << "    tri_ori= " << tri_ori << endl;
	cout << "    o0= " << o0 << endl;
	cout << "    o1= " << o1 << endl;
	cout << "    o2= " << o2 << endl;

	if (tri_ori >= 0.0f) {
		return o0 >= 0.0f and o1 >= 0.0f and o2 >= 0.0f;
	}
	return o0 <= 0.0f and o1 <= 0.0f and o2 <= 0.0f;
}

inline float triangle_area(const vec2& p, const vec2& q, const vec2& r) {
	vec3 p3(p, 0.0f);
	vec3 q3(q, 0.0f);
	vec3 r3(r, 0.0f);

	vec3 ij = q3 - p3;
	vec3 ik = r3 - p3;
	vec3 C = cross(ij, ik);
	return length(C)/2.0f;
}

// returns true if segment (p,q) intersects segment (r,s)
inline bool segment_intersection
(const vec2& p, const vec2& q, const vec2& r, const vec2& s)
{
	float ori1 = segment_orientation(p,q, r);
	float ori2 = segment_orientation(p,q, s);
	return ori1*ori2 <= 0.0f;
}

// linear search through all the meshe's triangles
inline int find_first_triangle
(const geoproc::TriangleMesh& mesh, const vector<vec2>& uvs, const vec2& p)
{
	for (int t = 0; t < mesh.n_triangles(); ++t) {
		int v0,v1,v2;
		mesh.get_vertices_triangle(t, v0,v1,v2);

		cout << "Checking triangle " << t << endl;
		cout << "    with vertices " << v0 << "," << v1 << "," << v2 << endl;
		cout << "    Is point (" << p.x << "," << p.y << ") inside this triangle?" << endl;

		if (inside_triangle(uvs, p, v0,v1,v2)) {
			cout << "    --> yes" << endl;
			return t;
		}
		cout << "    --> no" << endl;
	}

	// no triangle found
	return -1;
}

// locate the triangle where 'next' is in using the fact
// that point 'pre' is located in triangle 'pre_tri'
inline int find_next_triangle(
	const geoproc::TriangleMesh& mesh, int pre_tri,
	const vector<vec2>& uvs, const vec2& pre, const vec2& next
)
{
	const vector<geoproc::mesh_edge>& dcel = mesh.get_edges();
	const vector<int[3]>& edges_per_triangle = mesh.get_edges_triangle();

	// assume we have a segment from 'pre' to 'next'

	cout << "Find next triangle" << endl;

	int T = pre_tri;
	bool finish = false;
	while (not finish) {
		int vt0, vt1, vt2;
		mesh.get_vertices_triangle(T, vt0,vt1,vt2);

		cout << "    Checking triangle " << T << endl;
		cout << "    with vertices " << vt0 << "," << vt1 << "," << vt2 << endl;
		cout << "    Is point (" << next.x << "," << next.y << ") inside this triangle?" << endl;

		if (inside_triangle(uvs, next, vt0,vt1,vt2)) {
			finish = true;
			cout << "    --> yes. Finish" << endl;
			continue;
		}

		cout << "    --> no. Continue" << endl;

		// get edges and vertices of current triangle
		int e0 = edges_per_triangle[T][0];
		int v00 = dcel[e0].v0;
		int v01 = dcel[e0].v1;
		const vec2& p00 = uvs[v00];
		const vec2& p01 = uvs[v01];

		cout << "    edge " << e0 << ": " << v00 << " -> " << v01 << endl;
		cout << "        " << vec2_out(p00) << " -> " << vec2_out(p01) << endl;

		int e1 = edges_per_triangle[T][1];
		int v10 = dcel[e1].v0;
		int v11 = dcel[e1].v1;
		const vec2& p10 = uvs[v10];
		const vec2& p11 = uvs[v11];

		cout << "    edge " << e1 << ": " << v10 << " -> " << v11 << endl;
		cout << "        " << vec2_out(p10) << " -> " << vec2_out(p11) << endl;

		int e2 = edges_per_triangle[T][2];
		int v20 = dcel[e2].v0;
		int v21 = dcel[e2].v1;
		const vec2& p20 = uvs[v20];
		const vec2& p21 = uvs[v21];

		cout << "    edge " << e2 << ": " << v20 << " -> " << v21 << endl;
		cout << "        " << vec2_out(p20) << " -> " << vec2_out(p21) << endl;

		// check intersection between each edge of triangle
		// and the segment from 'pre' to 'next'
		if (segment_intersection(pre, next, p00, p01)) {
			cout << "Segment intersects edge " << e0 << endl;
			T = (dcel[e0].lT == T ? dcel[e0].rT : dcel[e0].lT);
		}
		else if (segment_intersection(pre, next, p10, p11)) {
			cout << "Segment intersects edge " << e1 << endl;
			T = (dcel[e1].lT == T ? dcel[e1].rT : dcel[e1].lT);
		}
		else if (segment_intersection(pre, next, p20, p21)) {
			cout << "Segment intersects edge " << e2 << endl;
			T = (dcel[e2].lT == T ? dcel[e2].rT : dcel[e2].lT);
		}
		else {
			cerr << "Error in" << endl;
			cerr << "    file: remeshing_harmonic_maps.cpp" << endl;
			cerr << "    line: " << __LINE__ << endl;
			cerr << "    When trying to locate point ("
				 << next.x << "," << next.y << ")" << endl;
			cerr << "    Triangle " << T << " does not intersect segment" << endl;
			cerr << "    (" << pre.x << "," << pre.y << ") -> "
				 << "(" << next.x << "," << next.y << ")" << endl;

			T = -1;
			finish = true;
		}

		cout << "    next triangle to look at: " << T << endl;
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
		vec2 pre(0.0f, 0.0f);
		vector<vec3> new_vertices(N*M);
		float w0,w1,w2;

		/* compute the coordinates of the new vertices */

		int nT = find_first_triangle(mesh, uvs, pre);
		if (nT == -1) {
			cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
			cerr << "    Could not locate point (0,0)." << endl;
			return false;
		}

		barycentric_coordinates(mesh, nT, uvs, pre, w0,w1,w2);
		make_new_vertex(mesh, nT, w0,w1,w2, new_vertices[it]);
		++it;

		cout << "Point (" << pre.x << "," << pre.y << ") is in triangle " << nT << endl;

		// first row of points
		for (size_t j = 1; j <= M; ++j) {
			vec2 next(0.0f, (1.0f*j)/M);
			nT = find_next_triangle(mesh, nT, uvs, pre, next);
			if (nT == -1) {
				cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
				cerr << "    Could not locate point (0," << j << ")= ("
					 << next.x << "," << next.y << ")" << endl;
				return false;
			}

			cout << "Point (" << next.x << "," << next.y << ") is in triangle " << nT << endl;

			barycentric_coordinates(mesh, nT, uvs, pre, w0,w1,w2);
			make_new_vertex(mesh, nT, w0,w1,w2, new_vertices[it]);
			++it;

			pre = next;
		}

		// rest of the grid
		for (size_t i = 1; i <= N; ++i) {
			for (size_t j = 0; j <= M; ++j) {
				vec2 next((1.0f*i)/N, (1.0f*j)/M);
				nT = find_next_triangle(mesh, nT, uvs, pre, next);
				if (nT == -1) {
					cerr << "geoproc::remeshing::harmonic_maps - Error" << endl;
					cerr << "    Could not locate point (" << i << "," << j << ")= ("
						 << next.x << "," << next.y << ")" << endl;
					return false;
				}

				barycentric_coordinates(mesh, nT, uvs, pre, w0,w1,w2);
				make_new_vertex(mesh, nT, w0,w1,w2, new_vertices[it]);
				++it;

				pre = next;
			}
		}

		/* compute new triangles */
		// this is easy because we handle only the 'Square' case
		vector<int> new_triangles;
		for (size_t i = 0; i <= N; ++i) {
			for (size_t j = 0; j <= M; ++j) {
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
