#include <geoproc/remeshing/remeshing.hpp>

// C++ includes
#include <vector>
using namespace std;

// glm includes
#include <glm/vec2.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/mesh_edge.hpp>

namespace geoproc {
namespace remeshing {

bool harmonic_maps(
	const TriangleMesh& m, size_t N, size_t M, const smoothing::smooth_weight& w,
	const parametrisation::boundary_shape& s, TriangleMesh& rm
)
{
	vector<vec2> uvs;
	bool r = parametrisation::harmonic_maps(m, w, s, uvs);
	if (not r) {
		return false;
	}

	const vector<mesh_edge>& dcel = m.get_edges();

	return true;
}

} // -- namespace remeshing
} // -- namespace geoproc
