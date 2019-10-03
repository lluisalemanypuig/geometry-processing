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

#include <geoproc/smoothing/local.hpp>

// C includes
#include <stdlib.h>
#include <omp.h>

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/smoothing/local_private.hpp>

namespace geoproc {
namespace smoothing {
namespace local {

void laplacian
(const weight& w, double l, size_t nit, TriangleMesh& m)
{
	const int N = m.n_vertices();

	// Allocate memory equivalent to two arrays of vertices.
	vec3d *verts = static_cast<vec3d *>(malloc(2*N*sizeof(vec3d)));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &verts[0]);

	bool res = local_private::apply_once_per_it(w, l, nit, m, &verts[0], &verts[N]);
	if (res) {
		m.set_vertices(&verts[N], N);
	}
	else {
		m.set_vertices(&verts[0], N);
	}

	// free memory
	free(verts);
}

void laplacian
(const weight& w, double l, size_t nit, size_t nt, TriangleMesh& m)
{
	if (nt == 1) {
		laplacian(w,l,nit, m);
		return;
	}

	const int N = m.n_vertices();

	// Allocate memory equivalent to two arrays of vertices.
	vec3d *verts = static_cast<vec3d *>(malloc(2*N*sizeof(vec3d)));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), verts);

	bool res = local_private::apply_once_per_it(w, l, nit, nt, m, &verts[0], &verts[N]);
	if (res) {
		m.set_vertices(&verts[N], N);
	}
	else {
		m.set_vertices(&verts[0], N);
	}

	// free memory
	free(verts);
}

void bilaplacian
(const weight& w, double l, size_t nit, TriangleMesh& m)
{
	const int N = m.n_vertices();

	// Allocate memory equivalent to two arrays of vertices.
	vec3d *verts = static_cast<vec3d *>(malloc(2*N*sizeof(vec3d)));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &verts[0]);

	local_private::apply_twice_per_it(w, l, -l, nit, m, &verts[0], &verts[N]);

	// Replace the coordinates of the old vertices in the
	// mesh for the vertices in 'old_verts'.
	m.set_vertices(&verts[0], N);

	// free memory
	free(verts);
}

void bilaplacian
(const weight& w, double l, size_t nit, size_t nt, TriangleMesh& m)
{
	if (nt == 1) {
		bilaplacian(w,l,nit, m);
		return;
	}

	const int N = m.n_vertices();

	// Allocate memory equivalent to two arrays of vertices.
	vec3d *verts = static_cast<vec3d *>(malloc(2*N*sizeof(vec3d)));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &verts[0]);

	local_private::apply_twice_per_it(w, l, -l, nit,  nt, m, &verts[0], &verts[N]);

	// Replace the coordinates of the old vertices in the
	// mesh for the vertices in 'old_verts'.
	m.set_vertices(&verts[0], N);

	// free memory
	free(verts);
}

void TaubinLM
(const weight& w, double l, size_t nit, TriangleMesh& m)
{
	const int N = m.n_vertices();
	const double Kpb = 0.1;
	const double mu = 1.0/(Kpb - 1.0/l);

	// Allocate memory equivalent to two arrays of vertices.
	vec3d *verts = static_cast<vec3d *>(malloc(2*N*sizeof(vec3d)));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &verts[0]);

	local_private::apply_twice_per_it(w, l, mu, nit, m, &verts[0], &verts[N]);

	// Replace the coordinates of the old vertices in the
	// mesh for the vertices in 'old_verts'.
	m.set_vertices(&verts[0], N);

	// free memory
	free(verts);
}

void TaubinLM
(const weight& w, double l, size_t nit, size_t nt, TriangleMesh& m)
{
	if (nt == 1) {
		TaubinLM(w,l,nit, m);
		return;
	}

	const int N = m.n_vertices();
	const double Kpb = 0.1;
	const double mu = 1.0/(Kpb - 1.0/l);

	// Allocate memory equivalent to two arrays of vertices.
	vec3d *verts = static_cast<vec3d *>(malloc(2*N*sizeof(vec3d)));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &verts[0]);

	local_private::apply_twice_per_it(w, l, mu, nit, nt, m, &verts[0], &verts[N]);

	// Replace the coordinates of the old vertices in the
	// mesh for the vertices in 'old_verts'.
	m.set_vertices(&verts[0], N);

	// free memory
	free(verts);
}

} // -- namespace local
} // -- namespace smoothing
} // -- namespace geoproc
