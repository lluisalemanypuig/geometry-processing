#include <geoproc/smoothing/global.hpp>

// C++ includes
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

// glm includes
#include <glm/vec3.hpp>
using namespace glm;

// geoproc includes
#include <geoproc/triangle_mesh.hpp>
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/smoothing/local_private.hpp>

// Eigen includes
#include <Eigen/Cholesky>

namespace geoproc {
namespace smoothing {
namespace global {

	static inline
	void get_weights
	(
		const smooth_weight& w, const TriangleMesh& m,
		vector<vector<float> >& ws
	)
	{
		if (w == smooth_weight::uniform) {
			for (int i = 0; i < m.n_vertices(); ++i) {
				local_private::make_uniform_weights(i, m, &(ws[i][0]));
				ws[i][i] = 1.0f;
			}
		}
		else if (w == smooth_weight::cotangent) {
			const vec3 *verts = &(m.get_vertices()[0]);
			for (int i = 0; i < m.n_vertices(); ++i) {
				local_private::make_cotangent_weights(i, m, verts, &(ws[i][0]));
				ws[i][i] = 1.0f;
			}
		}
	}

	void full_smooth
	(const smooth_operator& op, const smooth_weight& w, TriangleMesh& m)
	{

		if (op == smooth_operator::BiLaplacian) {
			cerr << "Error: global smoothing not implemented for 'BiLaplacian' operator"
				 << endl;
			return;
		}
		if (op == smooth_operator::TaubinLM) {
			cerr << "Error: global smoothing not implemented for 'TaubinLM' operator"
				 << endl;
			return;
		}

		const int N = m.n_vertices();

		vector<vector<float> > ws(N, vector<float>(N));
		get_weights(w, m, ws);

		vector<size_t> col_max_length(N, 0);

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				size_t l = std::to_string(ws[i][j]).length();
				col_max_length[j] = std::max(col_max_length[j], l);
			}
			col_max_length[i] = std::max(col_max_length[i], std::to_string(i).length());
		}

		cout << setw( std::to_string(N).length() ) << " " << " :";
		for (int i = 0; i < N; ++i) {
			cout << " " << setw(col_max_length[i]) << i;
		}
		cout << endl;
		for (int i = 0; i < N; ++i) {
			cout << setw( std::to_string(N).length() ) << i << " :";
			for (int j = 0; j < N; ++j) {
				cout << " " << setw(col_max_length[j]) << std::to_string(ws[i][j]);
			}
			cout << endl;
		}
		cout << endl;
	}


} // -- namespace global
} // -- namespace smoothing
} // -- namespace geoproc
