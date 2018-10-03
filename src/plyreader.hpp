#pragma once

// C++ includes
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Custom includes
#include "trianglemesh.hpp"

/**
 * @brief Read a mesh in .ply format
 *
 * In order to load a mesh in ply format call only function
 * @ref readMesh.
 */
namespace PLY_reader {
	bool __load_headerer(ifstream& fin, int& nVertices, int& nFaces);
	void __load_verticesces(ifstream& fin, int nVertices, vector<float>& plyVertices);
	void __load_facesces(ifstream& fin, int nFaces, vector<int>& plyTriangles);
	void __rescale_modeldel(vector<float>& plyVertices);
	void __add_model_to_mesh_mesh(const vector<float>& plyVertices, const vector<int>& plyTriangles, TriangleMesh& mesh);

	bool read_mesh(const QString& filename, TriangleMesh& mesh);

} // -- namespace PLY_reader
