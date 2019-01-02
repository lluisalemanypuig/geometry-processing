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

void remeshing_harmonic_maps(
	const TriangleMesh& m, const smoothing::smooth_weight& w,
	const parametrisation::boundary_shape& s, TriangleMesh& rm
)
{
	vector<vec2> uvs;
	parametrisation::harmonic_maps(m, w, s, uvs);

	const vector<mesh_edge>& dcel = m.get_edges();


}

} // -- namespace remeshing
} // -- namespace geoproc
