#pragma once

// geoproc includes
#include <geoproc/iterators/mesh_iterator.hpp>
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace iterators {
namespace vertex {

/**
 * @brief Vertex iterator class.
 *
 * Iterates over the vertices on the  1-ring
 * neighbourhood of a given vertex.
 */
class vertex_vertex_iterator : public mesh_iterator {
	private:
		/// Current corner index.
		int cur_corner;
		/// Current vertex index.
		int cur_vertex;

		/**
		 * @brief Should we do only half a step?
		 *
		 * In this case it means that we crossed a boundary,
		 * and retrieving the next vertex needs a slightly
		 * different procedure.
		 */
		bool half_step;

	public:
		/// Default constructor.
		vertex_vertex_iterator(const TriangleMesh& mesh);
		/// Destructor.
		~vertex_vertex_iterator();

		/**
		 * @brief Initialise iterator at a vertex.
		 *
		 * Modifies this class so that @ref current returns
		 * the first vertex on the 1-ring neighbourhood of @e v.
		 *
		 * @param v Index of the vertex. Must be a valid
		 * vertex index: 0 <= @e v < number of vertices.
		 * @returns Returns the first iterated vertex index.
		 * If this value is not collected, call @ref current,
		 * before calling @ref next.
		 */
		int init(int v);

		/**
		 * @brief Returns the current index of vertex.
		 * @pre This method must be called after initialising the
		 * iteratior with @ref init, and after having called, at
		 * least, method @ref next once.
		 * @return Returns the vertex index last returned by
		 * method @ref next.
		 */
		int current() const;

		/**
		 * @brief Returns the next vertex.
		 *
		 * The vertex index returned is in the 1-ring neighbourhood of
		 * the vertex used to initialise the iterator (see @ref init).
		 *
		 * The order of the vertices is guaranteed to be counterclockwise.
		 *
		 * @pre This method must be called after initialising the
		 * iteratior with @ref init.
		 * @return Returns a valid vertex index, except for those
		 * cases in which there is no next vertex (the case
		 * of a boundary). In such case, -1 is returned.
		 */
		int next();
};

/**
 * @brief Face iterator class.
 *
 * Iterates over the faces on the 1-ring
 * neighbourhood of a given vertex. These
 * are the faces adjacent to the given vertex.
 */
class vertex_face_iterator : public mesh_iterator {
	private:
		/// Current corner index.
		int cur_corner;
		/// Current face index.
		int cur_face;

	public:
		/// Default constructor.
		vertex_face_iterator(const TriangleMesh& mesh);
		/// Destructor.
		~vertex_face_iterator();

		/**
		 * @brief Initialise iterator at a vertex.
		 *
		 * Modifies this class so that @ref current returns
		 * the first face on the 1-ring neighbourhood of @e v.
		 *
		 * @param v Index of the vertex. Must be a valid
		 * vertex index: 0 <= @e v < number of vertices.
		 * @returns Returns the first iterated face index.
		 * If this value is not collected, call @ref current,
		 * before calling @ref next.
		 */
		int init(int v);

		/**
		 * @brief Returns the current index of vertex.
		 * @pre This method must be called after initialising the
		 * iteratior with @ref init, and after having called, at
		 * least, method @ref next once.
		 * @return Returns the face index last returned by
		 * method @ref next.
		 */
		int current() const;

		/**
		 * @brief Returns the next face.
		 *
		 * The face index returned is in the 1-ring neighbourhood of
		 * the vertex used to initialise the iterator (see @ref init).
		 *
		 * The order of the faces is guaranteed to be counterclockwise.
		 *
		 * @pre This method must be called after initialising the
		 * iteratior with @ref init.
		 * @return Returns a valid face index, except for those
		 * cases in which there is no next face (the case
		 * of a boundary). In such case, -1 is returned.
		 */
		int next();
};

} // -- namespace vertex
} // -- namespace iterators
} // -- namespace geoproc
