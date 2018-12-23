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
#include <glm/glm.hpp>
using namespace glm;

namespace geoproc {

// PRIVATE

void TriangleMesh::invalidate_areas_angles() {
	angles_area_valid = false;
	angles.clear();
	areas.clear();
}

void TriangleMesh::invalidate_neighbourhood() {
	neigh_valid = false;
	opposite_corners.clear();
	corners.clear();
	boundary_edges.clear();
}

void TriangleMesh::invalidate_boundaries() {
	boundaries_valid = false;
	boundaries.clear();
}

void TriangleMesh::invalidate_state() {
	invalidate_areas_angles();
	invalidate_neighbourhood();
}

// PROTECTED

void TriangleMesh::copy_mesh(const TriangleMesh& m) {
	destroy();

	vertices = m.vertices;
	triangles = m.triangles;
	normal_vectors = m.normal_vectors;
	opposite_corners = m.opposite_corners;
	corners = m.corners;
	boundaries = m.boundaries;

	min_coord = m.min_coord;
	max_coord = m.max_coord;

	angles = m.angles;
	areas = m.areas;

	neigh_valid = m.neigh_valid;
	angles_area_valid = m.angles_area_valid;
}

float TriangleMesh::get_triangle_area(int i, int j, int k) const {
	assert(0 <= i and i < n_vertices());
	assert(0 <= j and j < n_vertices());
	assert(0 <= k and k < n_vertices());

	vec3 ij = vertices[j] - vertices[i];
	vec3 ik = vertices[k] - vertices[i];
	vec3 c = cross(ij, ik);
	return length(c)/2.0f;
}

// PUBLIC

TriangleMesh::TriangleMesh() {
	neigh_valid = angles_area_valid = false;
}

TriangleMesh::TriangleMesh(const TriangleMesh& m) {
	copy_mesh(m);
}

TriangleMesh::~TriangleMesh() {
	destroy();
}

void TriangleMesh::set_vertices(const vector<float>& coords){
	assert(coords.size()%3 == 0);
	assert(coords.size() >= 2); // at least one vertex

	// initialise min,max coords so that valgrind doesn't cry
	min_coord.x = coords[0];
	min_coord.y = coords[1];
	min_coord.z = coords[2];
	max_coord.x = coords[0];
	max_coord.y = coords[1];
	max_coord.z = coords[2];

	vertices.resize(coords.size()/3);
	for (size_t i = 0; i < coords.size(); i += 3) {
		vertices[i/3].x = coords[i];
		vertices[i/3].y = coords[i+1];
		vertices[i/3].z = coords[i+2];

		min_coord = min(min_coord, vertices[i/3]);
		max_coord = min(max_coord, vertices[i/3]);
	}
	vertices.shrink_to_fit();

	invalidate_areas_angles();
}

void TriangleMesh::set_vertices(const vec3 *vs, int N) {
	assert(vs != nullptr);
	assert(N >= 1); // at least one vertex

	// initialise min,max coords so that valgrind doesn't cry
	min_coord = vs[0];
	max_coord = vs[0];

	vertices.resize(N);
	copy_if(
		vs, vs + N, vertices.begin(),
		[&](const vec3& v) -> bool {
			min_coord = min(min_coord, v);
			max_coord = min(max_coord, v);
			return true;
		}
	);
	vertices.shrink_to_fit();

	invalidate_areas_angles();
}

void TriangleMesh::set_vertices(const vector<vec3>& vs) {
	assert(vs.size() >= 1); // at least one vertex

	// initialise min,max coords so that valgrind doesn't cry
	min_coord = vs[0];
	max_coord = vs[0];

	vertices.resize(vs.size());
	copy_if(
		vs.begin(), vs.end(), vertices.begin(),
		[&](const vec3& v) -> bool {
			min_coord = min(min_coord, v);
			max_coord = min(max_coord, v);
			return true;
		}
	);
	vertices.shrink_to_fit();

	invalidate_areas_angles();
}

void TriangleMesh::set_triangles(const vector<int>& tris) {
	triangles.resize(tris.size());
	copy(tris.begin(), tris.end(), triangles.begin());
	// try optimising consumption of memory
	triangles.shrink_to_fit();

	invalidate_state();
}

void TriangleMesh::make_normal_vectors() {
	normal_vectors.resize(n_triangles());

	int v0,v1,v2;
	for (int t = 0; t < n_triangles(); ++t) {
		get_vertices_triangle(t, v0,v1,v2);
		normal_vectors[t] = normalize(cross(
			vertices[v1] - vertices[v0],
			vertices[v2] - vertices[v0]
		));
	}
}

void TriangleMesh::scale_to_unit() {
	vec3 center(0.0f, 0.0f, 0.0f);
	vec3 m(1e10, 1e10, 1e10);
	vec3 M(-1e10, -1e10, -1e10);

	for (size_t i = 0; i < vertices.size(); ++i) {
		center += vertices[i];
		m = min(m, vertices[i]);
		M = max(M, vertices[i]);
	}
	center /= vertices.size();

	float largestSize = std::max(M.x - m.x, std::max(M.y - m.y, M.z - m.z));

	for (unsigned int i = 0; i < vertices.size(); ++i) {
		vertices[i] = (vertices[i] - center)/largestSize;
	}

	invalidate_areas_angles();
}

void TriangleMesh::make_angles_area() {
	// compute this data only when needed
	if (angles_area_valid) {
		return;
	}

	angles.resize(triangles.size()/3);
	areas.resize(triangles.size()/3);

	vec3 u, v;
	for (size_t t = 0; t < triangles.size(); t += 3) {

		// fill vectors 'angles', 'areas'
		int i0 = triangles[t    ];
		int i1 = triangles[t + 1];
		int i2 = triangles[t + 2];

		areas[t/3] = get_triangle_area(i0,i1,i2);

		const vec3& v0 = get_vertex(i0);
		const vec3& v1 = get_vertex(i1);
		const vec3& v2 = get_vertex(i2);

		// angle <1,0,2>
		u = normalize( v1 - v0 );
		v = normalize( v2 - v0 );
		angles[t/3].x = acos( dot(u,v) );

		// angle <0,1,2>
		u = -u;
		v = normalize( v2 - v1 );
		angles[t/3].y = acos( dot(u,v) );

		// angle <1,2,0>
		u = normalize( v0 - v2 );
		v = -v;
		angles[t/3].z = acos( dot(u,v) );
	}
	// try optimising consumption of memory
	angles.shrink_to_fit();
	areas.shrink_to_fit();

	// validate areas and angles data
	angles_area_valid = true;
}

void TriangleMesh::destroy() {
	vertices.clear();
	triangles.clear();
	normal_vectors.clear();
	angles.clear();
	areas.clear();
	opposite_corners.clear();
	corners.clear();
	boundary_edges.clear();
	boundaries.clear();
	all_edges.clear();
	vertex_edge.clear();

	invalidate_state();
}

// GETTERS

int TriangleMesh::n_vertices() const {
	return static_cast<int>(vertices.size());
}

int TriangleMesh::n_edges() const {
	return static_cast<int>(all_edges.size());
}

int TriangleMesh::n_triangles() const {
	return static_cast<int>(triangles.size()/3);
}

int TriangleMesh::n_corners() const {
	return static_cast<int>(triangles.size());
}

int TriangleMesh::n_boundary_edges() const {
	return static_cast<int>(boundary_edges.size());
}

size_t TriangleMesh::n_boundaries() const {
	return boundaries.size();
}

int TriangleMesh::get_vertex_corner(int c) const {
	assert(0 <= c and c < n_corners());
	return triangles[c];
}

int TriangleMesh::get_corner_vertex(int v) const {
	assert(neigh_valid);
	assert(0 <= v and v < n_vertices());
	return corners[v];
}

int TriangleMesh::get_edge_vertex(int v) const {
	assert(neigh_valid);
	assert(0 <= v and v < n_vertices());
	return vertex_edge[v];
}

int TriangleMesh::get_triangle_corner(int c) const {
	assert(0 <= c and c < n_corners());
	return c/3;
}

void TriangleMesh::get_corners_triangle(int t, int& v0, int& v1, int& v2) const {
	assert(0 <= t and t < n_triangles());
	v0 = 3*t    ;
	v1 = 3*t + 1;
	v2 = 3*t + 2;
}

void TriangleMesh::get_vertices_triangle(int t, int& v0, int& v1, int& v2) const {
	assert(0 <= t and t < n_triangles());
	v0 = triangles[3*t    ];
	v1 = triangles[3*t + 1];
	v2 = triangles[3*t + 2];
}

void TriangleMesh::get_vertices_triangle(int t, int v, int& v0, int& v1, int& v2) const {
	assert(0 <= t and t < n_triangles());
	get_vertices_triangle(t, v0,v1,v2);

	// Resort the indexes.
	// It is easy to see that almost two
	// iterations will be done.
	int v3_copy;
	while (v0 != v) {
		v3_copy = v2;
		v2 = v1;
		v1 = v0;
		v0 = v3_copy;
	}
}

int TriangleMesh::get_opposite_corner(int c) const {
	assert(neigh_valid);
	assert(0 <= c and c < n_corners());
	return opposite_corners[c];
}

const vec3& TriangleMesh::get_vertex(int v) const {
	assert(0 <= v and v < n_vertices());
	return vertices[v];
}

const vector<vec3>& TriangleMesh::get_vertices() const {
	return vertices;
}

const vec3 *TriangleMesh::get_pvertices() const {
	return &vertices[0];
}

const vector<vec3>& TriangleMesh::get_normal_vectors() const {
	return normal_vectors;
}

float TriangleMesh::get_triangle_area(int t) const {
	assert(0 <= t and t < n_triangles());
	return areas[t];
}

const vector<float>& TriangleMesh::get_areas() const {
	assert(angles_area_valid);
	return areas;
}

const vector<vec3>& TriangleMesh::get_angles() const {
	assert(angles_area_valid);
	return angles;
}

bool TriangleMesh::are_angles_area_valid() const {
	return angles_area_valid;
}

bool TriangleMesh::is_neighbourhood_valid() const {
	return neigh_valid;
}

bool TriangleMesh::are_boundaries_valid() const {
	return boundaries_valid;
}

const std::vector<int>& TriangleMesh::get_vertex_edge() const {
	return vertex_edge;
}

const vector<mesh_edge>& TriangleMesh::get_edges() const {
	return all_edges;
}

const vector<int>& TriangleMesh::get_boundary_edges() const {
	return boundary_edges;
}

const vector<vector<int> >& TriangleMesh::get_boundaries() const {
	return boundaries;
}

void TriangleMesh::get_min_max_coordinates(vec3& m, vec3& M) const {
	m = min_coord;
	M = max_coord;
}

} // -- namespace geoproc
