#pragma once

// C includes
#include <string.h>

// C++ includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// glm includes
#include <glm/vec3.hpp>

// Custom includes
#include <algorithms/triangle_mesh.hpp>

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
	bool __load_header(ifstream& fin, int& n_verts, int& n_faces, string& format);

	/* Loading the vertices must be done accordingly to the format. */
	// BINARY format
	//     little endian 1.0
	void __load_vertices_binary_le_1_0(ifstream& fin, int n_verts, vector<float>& verts);
	void __load_faces_binary_le_1_0(ifstream& fin, int n_faces, vector<int>& tris);
	// ASCII format
	//     ascii 1.0
	void __load_vertices_ascii_1_0(ifstream& fin, int n_verts, vector<float>& verts);
	void __load_faces_ascii_1_0(ifstream& fin, int n_faces, vector<int>& tris);

	void __add_model_to_mesh(const vector<float>& verts, const vector<int>& tris, TriangleMesh& mesh);

	/**
	 * @brief Loads a triangular mesh stored in @e filename.
	 * @param filename File with the mesh in .ply format.
	 * @param[out] mesh YUUY
	 * @return Returns false on error.
	 */
	bool read_mesh(const string& filename, TriangleMesh& mesh);

} // -- namespace PLY_reader
