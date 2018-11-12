#include <geoproc/triangle_mesh.hpp>

// C includes
#include <assert.h>

// C++ includes
#include <functional>
#include <algorithm>
#include <iostream>
#include <cmath>

namespace geoproc {

struct CornerEdge {
	int vertexA, vertexB, corner;

	CornerEdge() : vertexA(-1), vertexB(-1), corner(-1) { }

	void set_indexes(int va, int vb, int c) {
		if (va < vb) {
			vertexA = va;
			vertexB = vb;
		}
		else {
			vertexA = vb;
			vertexB = va;
		}
		corner = c;
	}

	bool operator< (const CornerEdge& cEdge) const {
		return (vertexA < cEdge.vertexA) ||
			  ((vertexA == cEdge.vertexA) && (vertexB < cEdge.vertexB));
	}
	bool operator== (const CornerEdge& cEdge) const {
		return (vertexA == cEdge.vertexA) && (vertexB == cEdge.vertexB);
	}
};

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
	boundary.clear();
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
	boundary = m.boundary;

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

	glm::vec3 ij = vertices[j] - vertices[i];
	glm::vec3 ik = vertices[k] - vertices[i];
	glm::vec3 c = glm::cross(ij, ik);
	return glm::length(c)/2.0f;
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

void TriangleMesh::set_vertices(const std::vector<float>& coords){
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

		min_coord = glm::min(min_coord, vertices[i/3]);
		max_coord = glm::min(max_coord, vertices[i/3]);
	}
	vertices.shrink_to_fit();

	invalidate_areas_angles();
}

void TriangleMesh::set_vertices(const glm::vec3 *vs, int N) {
	assert(vs != nullptr);
	assert(N >= 1); // at least one vertex

	// initialise min,max coords so that valgrind doesn't cry
	min_coord = vs[0];
	max_coord = vs[0];

	vertices.resize(N);
	std::copy_if(
		vs, vs + N, vertices.begin(),
		[&](const glm::vec3& v) -> bool {
			min_coord = glm::min(min_coord, v);
			max_coord = glm::min(max_coord, v);
			return true;
		}
	);
	vertices.shrink_to_fit();

	invalidate_areas_angles();
}

void TriangleMesh::set_vertices(const std::vector<glm::vec3>& vs) {
	assert(vs.size() >= 1); // at least one vertex

	// initialise min,max coords so that valgrind doesn't cry
	min_coord = vs[0];
	max_coord = vs[0];

	vertices.resize(vs.size());
	std::copy_if(
		vs.begin(), vs.end(), vertices.begin(),
		[&](const glm::vec3& v) -> bool {
			min_coord = glm::min(min_coord, v);
			max_coord = glm::min(max_coord, v);
			return true;
		}
	);
	vertices.shrink_to_fit();

	invalidate_areas_angles();
}

void TriangleMesh::set_triangles(const std::vector<int>& tris) {
	triangles.resize(tris.size());
	std::copy(tris.begin(), tris.end(), triangles.begin());
	// try optimising consumption of memory
	triangles.shrink_to_fit();

	invalidate_state();
}

void TriangleMesh::make_normal_vectors() {
	normal_vectors.resize(n_triangles());

	int v0,v1,v2;
	for (int t = 0; t < n_triangles(); ++t) {
		get_vertices_triangle(t, v0,v1,v2);
		normal_vectors[t] = glm::normalize(glm::cross(
			vertices[v1] - vertices[v0],
			vertices[v2] - vertices[v0]
		));
	}
}

void TriangleMesh::scale_to_unit() {
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 m(1e10, 1e10, 1e10);
	glm::vec3 M(-1e10, -1e10, -1e10);

	for (size_t i = 0; i < vertices.size(); ++i) {
		center += vertices[i];
		m = glm::min(m, vertices[i]);
		M = glm::max(M, vertices[i]);
	}
	center /= vertices.size();

	float largestSize = std::max(M.x - m.x, std::max(M.y - m.y, M.z - m.z));

	for (unsigned int i = 0; i < vertices.size(); ++i) {
		vertices[i] = (vertices[i] - center)/largestSize;
	}

	invalidate_areas_angles();
}

void TriangleMesh::make_neighbourhood_data() {
	// compute this data only when needed
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
		// triangles[i+1]=j <-> "the i+1 corner of the
		// triangle has vertex j".

		int v0 = triangles[i];
		corners[v0] = i;

		int v1 = triangles[i + 1];
		corners[v1] = i + 1;

		int v2 = triangles[i + 2];
		corners[v2] = i + 2;
	}

	// ---------------------------
	// build opposite_corner table

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
	std::vector<CornerEdge> data;
	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		int c0 = i;
		int c1 = i + 1;
		int c2 = i + 2;

		CornerEdge e1, e2, e3;

		// edge v0,v1
		e1.set_indexes(triangles[c0], triangles[c1], c2);
		// edge v1,v2
		e2.set_indexes(triangles[c1], triangles[c2], c0);
		// edge v0,v2
		e3.set_indexes(triangles[c0], triangles[c2], c1);

		data.push_back(e1);
		data.push_back(e2);
		data.push_back(e3);
	}
	// sort the vector so that the edge pairs are consecutive
	sort(data.begin(), data.end());

	boundary.clear();
	// For each corner edge, check that a twin exists.
	// If so, we have found a pair of opposite corners
	for (size_t i = 0; i < data.size(); ++i) {
		// take corner indices
		int vA = data[i].vertexA;
		int vB = data[i].vertexB;

		if (data[i + 1].vertexA == vA and data[i + 1].vertexB == vB) {
			// a twin exists
			int o = data[i].corner;
			int c = data[i + 1].corner;
			opposite_corners[o] = c;
			opposite_corners[c] = o;

			// go to the next pair.
			++i;
		}
		else {
			// this single CornerEdge contains
			// an edge of the boundary -> hard boundary
			boundary.push_back( std::make_pair(vA, vB) );
			std::cout << "Boundary in edge (" << vA << "," << vB << ")" << std::endl;
			std::cout << "    "
					  << vertices[vA].x << ","
					  << vertices[vA].y << ","
					  << vertices[vA].z << std::endl;
			std::cout << "    "
					  << vertices[vB].x << ","
					  << vertices[vB].y << ","
					  << vertices[vB].z << std::endl;
		}
	}
	if (boundary.size() > 0) {
		std::cerr << "Warning: this mesh contains boundary(ies)" << std::endl;
		std::cerr << "    Computing Some curvatures may lead to"
				  << " a crash of the application" << std::endl;
	}

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
				std::cerr << "         corner " << i << " has opposite " << o << std::endl;
				std::cerr << "however, corner " << o << " has opposite " << c << std::endl;
				sane = false;
			}
		}
		++i;
	}
	assert( sane );
	#endif

	// validate neighbourhood data
	neigh_valid = true;
}

void TriangleMesh::make_angles_area() {
	// compute this data only when needed
	if (angles_area_valid) {
		return;
	}

	angles.resize(triangles.size()/3);
	areas.resize(triangles.size()/3);

	glm::vec3 u, v;
	for (size_t t = 0; t < triangles.size(); t += 3) {

		// fill vectors 'angles', 'areas'
		int i0 = triangles[t    ];
		int i1 = triangles[t + 1];
		int i2 = triangles[t + 2];

		areas[t/3] = get_triangle_area(i0,i1,i2);

		const glm::vec3& v0 = get_vertex(i0);
		const glm::vec3& v1 = get_vertex(i1);
		const glm::vec3& v2 = get_vertex(i2);

		// angle <1,0,2>
		u = glm::normalize( v1 - v0 );
		v = glm::normalize( v2 - v0 );
		angles[t/3].x = std::acos( glm::dot(u,v) );

		// angle <0,1,2>
		u = -u;
		v = glm::normalize( v2 - v1 );
		angles[t/3].y = std::acos( glm::dot(u,v) );

		// angle <1,2,0>
		u = glm::normalize( v0 - v2 );
		v = -v;
		angles[t/3].z = std::acos( glm::dot(u,v) );
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
	opposite_corners.clear();
	corners.clear();
	boundary.clear();
	angles.clear();
	areas.clear();

	invalidate_state();
}

// GETTERS

int TriangleMesh::n_vertices() const {
	return static_cast<int>(vertices.size());
}

int TriangleMesh::n_triangles() const {
	return static_cast<int>(triangles.size()/3);
}

int TriangleMesh::n_corners() const {
	return static_cast<int>(triangles.size());
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

int TriangleMesh::get_triangle_corner(int c) const {
	assert(0 <= c and c < n_corners());
	return c/3;
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

const glm::vec3& TriangleMesh::get_vertex(int v) const {
	assert(0 <= v and v < n_vertices());
	return vertices[v];
}

const std::vector<glm::vec3>& TriangleMesh::get_vertices() const {
	return vertices;
}

const glm::vec3 *TriangleMesh::get_pvertices() const {
	return &vertices[0];
}

float TriangleMesh::get_triangle_area(int t) const {
	assert(0 <= t and t < n_triangles());
	return areas[t];
}

const std::vector<float>& TriangleMesh::get_areas() const {
	assert(angles_area_valid);
	return areas;
}

const std::vector<glm::vec3>& TriangleMesh::get_angles() const {
	assert(angles_area_valid);
	return angles;
}

bool TriangleMesh::are_angles_area_valid() const {
	return angles_area_valid;
}

bool TriangleMesh::is_neighbourhood_valid() const {
	return neigh_valid;
}

void TriangleMesh::get_min_max_coordinates(glm::vec3& m, glm::vec3& M) const {
	m = min_coord;
	M = max_coord;
}

} // -- namespace geoproc
