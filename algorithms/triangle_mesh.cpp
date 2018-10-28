#include <algorithms/triangle_mesh.hpp>

#include <iostream>
using namespace std;

// LOCAL-DEFINED

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

void TriangleMesh::copy_mesh(const TriangleMesh& m) {
	destroy();

	vertices = m.vertices;
	triangles = m.triangles;
	opposite_corners = m.opposite_corners;
	corners = m.corners;
	boundary = m.boundary;
}

// PUBLIC

TriangleMesh::TriangleMesh() {

}

TriangleMesh::TriangleMesh(const TriangleMesh& m) {
	copy_mesh(m);
}

TriangleMesh::~TriangleMesh() {
	destroy();
}

void TriangleMesh::set_vertex(int vi, const glm::vec3& v) {
	assert(0 <= vi and vi < n_vertices());
	vertices[vi] = v;
}

void TriangleMesh::set_vertices(const vector<float>& coords){
	assert(coords.size()%3 == 0);
	vertices.resize(coords.size()/3);
	for (size_t i = 0; i < coords.size(); i += 3) {
		vertices[i/3] = glm::vec3(coords[i], coords[i+1], coords[i+2]);
	}
	vertices.shrink_to_fit();
}

void TriangleMesh::set_vertices(const glm::vec3 *vs, int N) {
	assert(vs != nullptr);
	vertices.resize(N);
	std::copy(&vs[0], &vs[N - 1], vertices.begin());
	vertices.shrink_to_fit();
}

void TriangleMesh::set_vertices(const vector<glm::vec3>& vs) {
	vertices.resize(vs.size());
	std::copy(vs.begin(), vs.end(), vertices.begin());
	vertices.shrink_to_fit();
}

void TriangleMesh::set_triangles(const vector<int>& tris) {
	triangles.resize(tris.size());
	std::copy(tris.begin(), tris.end(), triangles.begin());
	triangles.shrink_to_fit();
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

	float largestSize = max(M.x - m.x, max(M.y - m.y, M.z - m.z));

	for (unsigned int i = 0; i < vertices.size(); ++i) {
		vertices[i] = (vertices[i] - center)/largestSize;
	}
}

void TriangleMesh::make_neighbourhood_data() {
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
	vector<CornerEdge> data;
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
			boundary.push_back( make_pair(vA, vB) );
			cout << "Boundary in edge (" << vA << "," << vB << ")" << endl;
			cout << "    "
					  << vertices[vA].x << ","
					  << vertices[vA].y << ","
					  << vertices[vA].z << endl;
			cout << "    "
					  << vertices[vB].x << ","
					  << vertices[vB].y << ","
					  << vertices[vB].z << endl;
		}
	}
	if (boundary.size() > 0) {
		cerr << "Warning: this mesh contains boundary(ies)" << endl;
		cerr << "    Computing Some curvatures may lead to a crash of the application" << endl;
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
				cerr << "         corner " << i << " has opposite " << o << endl;
				cerr << "however, corner " << o << " has opposite " << c << endl;
				sane = false;
			}
		}
		++i;
	}
	assert( sane );
	#endif
}

void TriangleMesh::destroy() {
	vertices.clear();
	triangles.clear();
	opposite_corners.clear();
	corners.clear();
	boundary.clear();
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
	assert(0 <= c and c < n_corners());
	return opposite_corners[c];
}

const glm::vec3& TriangleMesh::get_vertex(int v) const {
	assert(0 <= v and v < n_vertices());
	return vertices[v];
}

const vector<glm::vec3>& TriangleMesh::get_vertices() const {
	return vertices;
}

float TriangleMesh::get_triangle_area(int t) const {
	assert(0 <= t and t < n_triangles());
	int i, j, k;
	get_vertices_triangle(t, i,j,k);
	return get_triangle_area(i,j,k);
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
