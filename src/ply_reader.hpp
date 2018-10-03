#pragma once

// C++ includes
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Custom includes
#include "triangle_mesh.hpp"

/**
 * @brief Read a mesh in .ply format
 *
 * In order to load a mesh in ply format call only function
 * @ref read_mesh.
 */
namespace PLY_reader {
	bool __load_header(ifstream& fin, int& nVertices, int& nFaces);
	void __load_vertices(ifstream& fin, int nVertices, vector<float>& plyVertices);
	void __load_faces(ifstream& fin, int nFaces, vector<int>& plyTriangles);
	void __rescale_model(vector<float>& plyVertices);
	void __add_model_to_mesh(const vector<float>& plyVertices, const vector<int>& plyTriangles, TriangleMesh& mesh);

	/**
	 * @brief Loads a triangular mesh stored in @e filename.
	 * @param filename File with the mesh in .ply format.
	 * @param[out] mesh YUUY
	 * @return Returns false on error.
	 */
	bool read_mesh(const QString& filename, TriangleMesh& mesh);

} // -- namespace PLY_reader
