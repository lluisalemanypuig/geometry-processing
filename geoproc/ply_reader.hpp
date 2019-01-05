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
