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

#include <geoproc/mesh_edge.hpp>

namespace geoproc {

mesh_edge::mesh_edge() {
	v0 = v1 = lT = rT = pE = nE = -1;
}

mesh_edge::mesh_edge(int pv, int nv, int lt, int rt) {
	v0 = pv;
	v1 = nv;
	lT = lt;
	rT = rt;
	pE = nE = -1;
}

mesh_edge::mesh_edge(const mesh_edge& m) {
	v0 = m.v0;
	v1 = m.v1;
	lT = m.lT;
	rT = m.rT;
	pE = m.pE;
	nE = m.nE;
}

mesh_edge::~mesh_edge() {
}

} // -- namespace geoproc
