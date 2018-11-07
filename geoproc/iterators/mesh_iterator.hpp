#pragma once

// C++ includes

// geoproc includes
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {
namespace iterators {

/**
 * @brief Abstract iterator class.
 *
 * Defines three methods
 * - @ref init
 * - @ref current
 * - @ref next
 *
 * that have to be implemented in order to define the
 * iteration on the mesh.
 *
 * Each class that implements this one must document
 * the behaviour of their implementations.
 */
class mesh_iterator {
	protected:
		/// Constant reference to iterated mesh.
		const TriangleMesh& mesh;

	public:
		/// Constructor.
		mesh_iterator(const TriangleMesh& m);
		/// Destructor.
		virtual ~mesh_iterator();

		/// Initialise iterator with an index.
		virtual int init(int idx) = 0;

		/// Return index to current element.
		virtual int current() const = 0;

		/**
		 * @brief Advances the iterator and returns the
		 * currently pointed element.
		 *
		 * The iterator advances to a state in which it points
		 * to some element. The value returned is an index to that
		 * element.
		 */
		virtual int next() = 0;
};

} // -- namespace iterators
} // -- namespace geoproc
