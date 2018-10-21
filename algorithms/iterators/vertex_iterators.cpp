#include <algorithms/iterators/vertex_iterators.hpp>

inline
int next_corner(int corner) {
	return 3*(corner/3) + (corner + 1)%3;
}

inline
int previous_corner(int corner) {
	return 3*(corner/3) + (corner + 2)%3;
}

namespace iterators {
namespace vertex {

/* ---------------------- */
/* VERTEX-VERTEX ITERATOR */

// PUBLIC

vertex_vertex_iterator::vertex_vertex_iterator(const TriangleMesh& m)
	: mesh_iterator(m)
{
	cur_corner = -1;
	cur_vertex = -1;
}

vertex_vertex_iterator::~vertex_vertex_iterator() {}

int vertex_vertex_iterator::init(int c) {
	assert(c != -1);

	// copy initialiser corner and find current element
	cur_corner = mesh.get_corner_vertex(c);
	return next();
}

int vertex_vertex_iterator::current() const {
	return cur_vertex;
}

int vertex_vertex_iterator::next() {
	if (cur_corner == -1) {
		return -1;
	}

	// find next corner of the current corner
	int ncc = next_corner(cur_corner);
	// retrieve vertex index
	cur_vertex = mesh.get_vertex_corner(ncc);

	// find corner opposite to the next corner of the current corner
	int o = mesh.get_opposite_corner(ncc);
	// update current corner
	cur_corner = next_corner(o);

	return cur_vertex;
}

/* -------------------- */
/* VERTEX-FACE ITERATOR */

vertex_face_iterator::vertex_face_iterator(const TriangleMesh& m)
	: mesh_iterator(m)
{
	cur_corner = -1;
	cur_face = -1;
}

vertex_face_iterator::~vertex_face_iterator() {}

int vertex_face_iterator::init(int c) {
	assert(c != -1);

	// copy initialiser corner and find current element
	cur_corner = mesh.get_corner_vertex(c);
	return next();
}

int vertex_face_iterator::current() const {
	return cur_face;
}

int vertex_face_iterator::next() {
	if (cur_corner == -1) {
		return -1;
	}

	// retrieve verteface index
	cur_face = mesh.get_face_corner(cur_corner);

	// find next corner of the current corner
	int next_current = next_corner(cur_corner);
	// find corner opposite to the next corner of the current corner
	int o = mesh.get_opposite_corner(next_current);
	// update current corner
	cur_corner = next_corner(o);

	return cur_face;
}

} // -- namespace vertex
} // -- namespace iterators
