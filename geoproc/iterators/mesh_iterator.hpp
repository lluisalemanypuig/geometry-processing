/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

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
		/**
		 * @brief Default constructor.
		 * @param m Constant reference to the iterated mesh.
		 * @pre The mesh requires:
		 * - Neighbourhood data (see @ref TriangleMesh::make_neighbourhood_data)
		 */
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
