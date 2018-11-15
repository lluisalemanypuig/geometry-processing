#include <geoproc/iterators/vertex_iterators.hpp>

// C includes
#include <assert.h>

#include <iostream>
using namespace std;

inline
int next_corner(int corner) {
	return 3*(corner/3) + (corner + 1)%3;
}

inline
int previous_corner(int corner) {
	return 3*(corner/3) + (corner + 2)%3;
}

namespace geoproc {
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
	half_step = false;
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

	if (half_step) {
		int nc = next_corner(cur_corner);
		int nc2 = next_corner(nc);
		cur_vertex = mesh.get_vertex_corner(nc2);

		int o = -mesh.get_opposite_corner(nc);
		cur_corner = next_corner(o);

		half_step = false;
		return cur_vertex;
	}

	// find next corner of the current corner
	int ncc = next_corner(cur_corner);

	// retrieve vertex index
	cur_vertex = mesh.get_vertex_corner(ncc);

	// find corner opposite to the next
	// corner of the current corner
	int o = mesh.get_opposite_corner(ncc);
	if (o < 0) {
		// boundary of the mesh!
		// Here we have to be careful when taking the
		// next step, since we are crossing a boundary.
		half_step = true;
	}
	else {
		// update current corner
		cur_corner = next_corner(o);
	}

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

	// retrieve face index
	cur_face = mesh.get_triangle_corner(cur_corner);

	// find next corner of the current corner
	int next_current = next_corner(cur_corner);
	// find corner opposite to the next corner of the current corner
	int o = mesh.get_opposite_corner(next_current);
	if (o < 0) {
		// boundary of the mesh!
		// Here we have to be careful when taking the
		// next step, since we are crossing a boundary.
		cur_corner = next_corner(-o);
	}
	else {
		// update current corner
		cur_corner = next_corner(o);
	}

	return cur_face;
}

} // -- namespace vertex
} // -- namespace iterators
} // -- namespace geoproc
