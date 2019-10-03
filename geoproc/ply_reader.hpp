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
#include <string>
#include <vector>

// geoproc includes
#include <geoproc/triangle_mesh.hpp>

namespace geoproc {

/**
 * @brief Read a mesh in .ply format
 *
 * In order to load a mesh in ply format call only function
 * @ref read_mesh(const std::string&, TriangleMesh&).
 * 
 * This reader only supports those files that do not contain
 * anything else than vertex coordinates and the description
 * of every face.
 */
namespace PLY_reader {

	/**
	 * @brief Loads a triangular mesh stored in @e filename.
	 * @param filename File with the mesh in .ply format.
	 * @param[out] mesh Mesh loaded from file.
	 * @return Returns false on error.
	 */
	bool read_mesh(const std::string& filename, TriangleMesh& mesh);

} // -- namespace PLY_reader
} // -- namespace geoproc
