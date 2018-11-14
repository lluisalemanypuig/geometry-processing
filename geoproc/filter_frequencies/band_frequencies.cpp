#include <geoproc/filter_frequencies/band_frequencies.hpp>

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

namespace filter_frequencies {

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

vec3 *apply_conf_get_verts
(
	const smoothing_configuration& S,
	const TriangleMesh& m,
	size_t nt,
	vec3 *old_verts,
	vec3 *new_verts
)
{
	if (S.so == smooth_operator::Laplacian) {
		bool res = smoothing_private::apply_once_per_it
			(S.sw, S.lambda, S.n_it, nt, m, old_verts, new_verts);

		return (res ? new_verts : old_verts);
	}
	if (S.so == smooth_operator::BiLaplacian) {
		smoothing_private::apply_twice_per_it
			(S.sw, S.lambda, -S.lambda, S.n_it, nt, m, old_verts, new_verts);

		return old_verts;
	}
	if (S.so == smooth_operator::TaubinLM) {
		const float Kpb = 0.1f;
		const float mu = 1.0f/(Kpb - 1.0f/S.lambda);
		smoothing_private::apply_twice_per_it
			(S.sw, S.lambda, mu, S.n_it, nt, m, old_verts, new_verts);

		return old_verts;
	}

	return old_verts;
}

/* BAND FREQUENCIES
 */

void band_frequencies
(
	const vector<smoothing_configuration>& confs,
	const vector<float>& mus,
	TriangleMesh& m
)
{
	assert(mus.size() == confs.size() - 1);

	const int N = m.n_vertices();

	// copy original vertices, store result of band frequencies here
	vector<vec3> out_verts = m.get_vertices();

	// Allocate memory for "four" arrays of vertices. Actually, there are only
	// two, but each pair is packed together into a single one.
	// They will be used for computing the different smoothings of the mesh
	// while avoiding recomputing old results.
	// Each array is used as two, namely, the interval [0,N) corresponds
	// to a series of vertices, and the interval [N,2*N) to another series
	// of vertices.
	vec3 *vertsi  = static_cast<vec3 *>(malloc(2*N*sizeof(vec3)));
	vec3 *vertsi1 = static_cast<vec3 *>(malloc(2*N*sizeof(vec3)));

	// Fill the array i
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &vertsi[0]);
	// apply configuration i
	vec3 *outi = apply_conf_get_verts(confs[0], m, &vertsi[0], &vertsi[N]);

	for (size_t i = 1; i < confs.size(); ++i) {

		// Fill the array i+1
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), vertsi1);
		// apply configuration i+1
		vec3 *outi1 = apply_conf_get_verts(confs[i], m, &vertsi1[0], &vertsi1[N]);

		// operate: M += mu_i*(smooth_i - smooth_i+1)
		for (int j = 0; j < m.n_vertices(); ++j) {
			out_verts[j] = out_verts[j] + mus[i - 1]*(outi[j] - outi1[j]);
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

void band_frequencies
(
	const vector<smoothing_configuration>& confs,
	const vector<float>& mus, size_t nt,
	TriangleMesh& m
)
{
	assert(mus.size() == confs.size() - 1);

	if (nt == 1) {
		band_frequencies(confs, mus, m);
		return;
	}

	const int N = m.n_vertices();

	// copy original vertices, store result of band frequencies here
	vector<vec3> out_verts = m.get_vertices();

	// Allocate memory for "four" arrays of vertices. Actually, there are only
	// two, but each pair is packed together into a single one.
	// They will be used for computing the different smoothings of the mesh
	// while avoiding recomputing old results
	// Each array is used as two, namely, the interval [0,N) corresponds
	// to a series of vertices, and the interval [N,2*N) to another series
	// of vertices.
	vec3 *vertsi  = static_cast<vec3 *>(malloc(2*N*sizeof(vec3)));
	vec3 *vertsi1 = static_cast<vec3 *>(malloc(2*N*sizeof(vec3)));

	// Fill the array i
	std::copy(m.get_vertices().begin(), m.get_vertices().end(), &vertsi[0]);
	// apply configuration i
	vec3 *outi = apply_conf_get_verts(confs[0], m, nt, &vertsi[0], &vertsi[N]);

	for (size_t i = 1; i < confs.size(); ++i) {

		// Fill the array i+1
		std::copy(m.get_vertices().begin(), m.get_vertices().end(), vertsi1);
		// apply configuration i+1
		vec3 *outi1 = apply_conf_get_verts(confs[i], m, nt, &vertsi1[0], &vertsi1[N]);

		// operate: M += mu_i*(smooth_i - smooth_i+1)
		#pragma omp parallel for num_threads(nt)
		for (int j = 0; j < m.n_vertices(); ++j) {
			out_verts[j] = out_verts[j] + mus[i - 1]*(outi[j] - outi1[j]);
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

} // -- namespace filter_frequencies
} // -- namespace geoproc
