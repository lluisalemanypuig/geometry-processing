#pragma once

// geoproc includes
#include <geoproc/smoothing/smoothing_defs.hpp>
#include <geoproc/parametrisation/parametrisation_defs.hpp>
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace remeshing {

	/**
	 * @brief Remeshes a triangle mesh using harmonic map coordinates.
	 *
	 * First, computes the harmonic maps parametrisation of the mesh, which
	 * yields a set of 2d coordinates in the xy plane, more concretely in
	 * the square [0,1]x[0,1] (regardless of the shape).
	 *
	 * Then, in this square is generated a NxM grid of points. The point
	 * \f$P_{ij}\f$ (with \f$0\le i < N, \quad 0\le j < M\f$) has coordinates
	 * \f$(x(P_{ij}), y(P_{ij})) = (i/N, j/M)\f$.
	 *
	 * Each of these points is located in the triangulation obtained from the
	 * parametrisation. That means, that we know the triangle of the
	 * triangulation each point of the grid is in.
	 *
	 * With this information, for each point in the grid \f$P_{ij}\f$, inside
	 * triangle \f$t_k\f$, the algorithm computes the contribution of the
	 * vertices of \f$t_k\f$ to make point \f$P_{ij}\f$. That is, it computes
	 * the weights \f$w_1,w_2,w_3 > 0, w_1 + w_2 + w_3 = 1\f$ such that
	 *
	 * \f$
	 * P_{ij} = w_1 \cdot v_1(t_k) + w_2 \cdot v_2(t_k) + w_2 \cdot v_2(t_k)
	 * \f$
	 *
	 * where \f$v_s(t_k)\f$ is the @e s-th vertex of triangle \f$t_k\f$ in the
	 * triangulation obtained from the parametrisation.
	 *
	 * Once all the \f$w_i\f$ are known for each point in the grid, the new
	 * mesh is computed: a new 3-dimensional point is made for each point in
	 * the grid, using the weights \f$w_1,w_2,w_3\f$ calculated before:
	 *
	 * \f$
	 * Q_{ij} = w_1 \cdot v_1(T_k) + w_2 \cdot v_2(T_k) + w_3 \cdot v_3(T_k)
	 * \f$
	 *
	 * where \f$T_k\f$ denotes the triangle of the original mesh @e m that
	 * is mapped to triangle \f$t_k\f$ due to the parametrisation, which is,
	 * in turn, the triangle that encloses point \f$P_{ij}\f$.
	 * @param[in] m Triangle mesh to be remeshed.
	 * @param[in] N Size of the grid in the x-axis.
	 * @param[in] M Size of the grid in the y-axis.
	 * @param[in] w Type of weight to use to obtain the parametrisation.
	 * @param[in] s Boundary shape of the parametrisation.
	 * @param[out] rm Remeshed mesh.
	 * @pre The input mesh needs all of its edges computed, namely, neighbourhood
	 * data has to be valid.
	 * @pre The preconditions of the method @ref parametrisation::harmonic_maps
	 * have to be true before calling this method.
	 */
	void remeshing_harmonic_maps
	(const TriangleMesh& m, size_t N, size_t M, const smoothing::smooth_weight& w,
	 const parametrisation::boundary_shape& s, TriangleMesh& rm);

} // -- namespace remeshing
} // -- namespace geoproc
