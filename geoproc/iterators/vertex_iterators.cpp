/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

#include <geoproc/iterators/vertex_iterators.hpp>

// C includes
#include <assert.h>

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

int vertex_vertex_iterator::init(int v) {
	assert(v >= 0);

	cur_corner = -1;
	cur_vertex = -1;
	half_step = false;

	// copy initialiser corner and find current element
	cur_corner = mesh.get_corner_vertex(v);
	return next();
}

int vertex_vertex_iterator::current() const {
	return cur_vertex;
}

int vertex_vertex_iterator::next() {
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

int vertex_face_iterator::init(int v) {
	assert(v >= 0);

	cur_corner = -1;
	cur_face = -1;

	// copy initialiser corner and find current element
	cur_corner = mesh.get_corner_vertex(v);
	return next();
}

int vertex_face_iterator::current() const {
	return cur_face;
}

int vertex_face_iterator::next() {
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
