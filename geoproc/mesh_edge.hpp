#pragma once

namespace geoproc {

	/**
	 * @brief Meshe's edge defition.
	 *
	 * It is basically a set of four indices pointing to the first
	 * and last vertices (order decided arbitrarily), and to the left
	 * and right faces (with respect to the previous and next vertices).
	 *
	 * This is defined as a 'class' just so as not to have to type
	 * 'struct' (oops, I typed 'struct', oops I did it again!).
	 */
	class mesh_edge {
		public:
			/**
			 * @brief First vertex's index.
			 *
			 * The edge is oriented from @ref v0 to @ref v1.
			 */
			int v0;
			/**
			 * @brief Last vertex's index.
			 *
			 * The edge is oriented from @ref v0 to @ref v1.
			 */
			int v1;
			/**
			 * @brief Left triangle index.
			 *
			 * The incident triangle to this edge to the left of
			 * the oriented segment from the previous to the next vertex.
			 */
			int lT;
			/**
			 * @brief Right triangle index.
			 *
			 * The incident triangle to this edge to the right of
			 * the oriented segment from the previous to the next vertex.
			 */
			int rT;

			/**
			 * @brief The previous edge to this.
			 *
			 * The first edge found when iterating in counterclockwise order
			 * around vertex @ref v0, starting at this edge.
			 */
			int pE;
			/**
			 * @brief The next edge to this.
			 *
			 * The first edge found when iterating in counterclockwise order
			 * around vertex @ref v1, starting at this edge.
			 */
			int nE;

			/// Default constructor.
			mesh_edge();
			/// Constructor with indices (1).
			mesh_edge(int pv, int nv, int lt, int rt);
			/// Copy constructor.
			mesh_edge(const mesh_edge& m);
			/// Destructor
			~mesh_edge();
	};

} // -- namespace geoproc
