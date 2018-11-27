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
 * @ref read_mesh.
 * 
 * This reader only supports those files that do not contain
 * anything else than vertex coordinates and the description
 * of every face.
 */
namespace PLY_reader {
	bool __load_header(std::ifstream& fin, int& n_verts, int& n_faces, std::string& format);

	/* Loading the vertices must be done accordingly to the format. */
	// BINARY format
	//     little endian 1.0
	void __load_vertices_binary_le_1_0(std::ifstream& fin, int n_verts, std::vector<float>& verts);
	void __load_faces_binary_le_1_0(std::ifstream& fin, int n_faces, std::vector<int>& tris);
	// ASCII format
	//     ascii 1.0
	void __load_vertices_ascii_1_0(std::ifstream& fin, int n_verts, std::vector<float>& verts);
	void __load_faces_ascii_1_0(std::ifstream& fin, int n_faces, std::vector<int>& tris);

	void __add_model_to_mesh
	(const std::vector<float>& verts, const std::vector<int>& tris, TriangleMesh& mesh);

	/**
	 * @brief Loads a triangular mesh stored in @e filename.
	 * @param filename File with the mesh in .ply format.
	 * @param[out] mesh Mesh loaded from file.
	 * @return Returns false on error.
	 */
	bool read_mesh(const std::string& filename, TriangleMesh& mesh);

} // -- namespace PLY_reader
} // -- namespace geoproc
