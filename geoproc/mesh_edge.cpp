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
