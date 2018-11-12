#include <geoproc/high-frequencies/highfrequencies.hpp>

// C includes
#include <stdlib.h>

// C++ includes
#include <vector>
using namespace std;

// glm includes
#include <glm/vec3.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/smoothing/iterative_private.hpp>

namespace geoproc {
using namespace smoothing;

namespace high_frequencies {

vec3 *apply_conf_get_verts
(
	const smoothing_configuration& S,
	const TriangleMesh& m,
	vec3 *old_verts,
	vec3 *new_verts
)
{
	if (S.so == smooth_operator::Laplacian) {
		bool res = smoothing_private::apply_once_per_it
			(S.sw, S.lambda, S.n_it, m, old_verts, new_verts);

		return (res ? new_verts : old_verts);
	}
	if (S.so == smooth_operator::BiLaplacian) {
		smoothing_private::apply_twice_per_it
			(S.sw, S.lambda, -S.lambda, S.n_it, m, old_verts, new_verts);

		return old_verts;
	}
	if (S.so == smooth_operator::TaubinLM) {
		const float Kpb = 0.1f;
		const float mu = 1.0f/(Kpb - 1.0f/S.lambda);
		smoothing_private::apply_twice_per_it
			(S.sw, S.lambda, mu, S.n_it, m, old_verts, new_verts);

		return old_verts;
	}

	return old_verts;
}

void high_frequency_details(const smoothing_configuration& S, TriangleMesh& m) {
	const int N = m.n_vertices();

	// Allocate memory for two arrays of vertices.
	vec3 *old_verts = (vec3 *)malloc(N*sizeof(vec3));
	vec3 *new_verts = (vec3 *)malloc(N*sizeof(vec3));
	// Fill the first array (there is no need to fill the second).
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), old_verts);

	// output vertices
	vec3 *out_verts = apply_conf_get_verts(S, m, old_verts, new_verts);

	// old_vertices contained, at first, the vertices of the mesh.
	// Now contain rubbish.

	const vec3 *m_verts = m.get_pvertices();
	for (int i = 0; i < N; ++i) {
		// out verts is initialised since we assign to it
		// the array with the vertices resulting from the
		// smoothing process.
		out_verts[i] = m_verts[i] - out_verts[i];
	}

	m.set_vertices(out_verts, N);

	// free memory: out_verts is just a pointer to the
	// one of the two arrays, no need to free it
	free(old_verts);
	free(new_verts);
}

void exaggerate_high_frequencies(const smoothing_configuration& S, TriangleMesh& m) {
	// copy original vertices
	vector<vec3> orig_verts = m.get_vertices();

	// apply high frequency details...
	high_frequency_details(S, m);

	// ... then exaggerate
	vector<vec3> m_verts(m.n_vertices());
	for (int i = 0; i < m.n_vertices(); ++i) {
		m_verts[i] = orig_verts[i] + S.lambda*m.get_vertex(i);
	}

	m.set_vertices(m_verts);
}

void band_frequencies(const vector<smoothing_configuration>& confs, TriangleMesh& m) {
	const int N = m.n_vertices();

	// copy original vertices
	vector<vec3> out_verts = m.get_vertices();

	// Allocate memory for "four" arrays of vertices. Actually, there are only
	// two, but each pair is packed together into a single one.
	// They will be used for computing the different smoothings of the mesh
	// while avoiding recomputing old results
	vec3 *vertsi  = (vec3 *)malloc(2*N*sizeof(vec3));
	vec3 *vertsi1 = (vec3 *)malloc(2*N*sizeof(vec3));

	// Fill the array i
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &vertsi[0]);
	// apply configuration i
	vec3 *outi = apply_conf_get_verts(confs[0], m, &vertsi[0], &vertsi[N]);

	for (size_t i = 1; i < confs.size() - 1; ++i) {

		// Fill the array i+1
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), vertsi1);
		// apply configuration i+1
		vec3 *outi1 = apply_conf_get_verts(confs[i], m, &vertsi1[0], &vertsi1[N]);

		// operate: M += lambda_i*(smooth_i - smooth_i+1)
		for (int j = 0; j < m.n_vertices(); ++j) {
			out_verts[j] = out_verts[j] + confs[i - 1].lambda*(outi[j] - outi1[j]);
		}

		// Copy the computed results so as to use them in the next iteration.
		// New results are stored in [outi1, outi1 + N)
		std::copy(outi1, outi1 + N, &vertsi[0]);
	}

	m.set_vertices(out_verts);

	// free memory: outi and outi1 are just pointers
	// to the two arrays, no need to free it
	free(vertsi);
	free(vertsi1);
}

} // -- namespace high_frequencies
} // -- namespace geoproc
