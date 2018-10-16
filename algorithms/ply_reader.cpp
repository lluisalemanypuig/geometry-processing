#include "ply_reader.hpp"

namespace PLY_reader {

	bool __load_header(ifstream& fin, int& nVertices, int& nFaces, string& format) {
		char line[100];

		fin.getline(line, 100);
		if (strncmp(line, "ply", 3) != 0) {
			cerr << "    PLY_reader::__load_header - Error:" << endl;
			cerr << "        Wrong format of file: first line does not contain 'ply'." << endl;
			return false;
		}
		nVertices = 0;
		fin.getline(line, 100);
		while (strncmp(line, "end_header", 10) != 0) {

			if (strncmp(line, "element face", 12) == 0) {
				nFaces = atoi(&line[13]);
			}
			else if (strncmp(line, "element vertex", 14) == 0) {
				nVertices = atoi(&line[15]);
			}
			else if (strncmp(line, "format", 6) == 0) {
				format = string(&line[7]);
			}
			else if (strncmp(line, "property float nx", 17) == 0) {
				cerr << "        PLY_reader::__load_header - Error:" << endl;
				cerr << "            This model has normals: more vertices than necessary" << endl;
				cerr << "            are in the file and this makes it too difficult to" << endl;
				cerr << "            create the mesh." << endl;
				return false;
			}
			fin.getline(line, 100);
		}
		if (nVertices <= 0) {
			cerr << "        PLY_reader::__load_header - Error:" << endl;
			cerr << "            Number of vertices read is negative." << endl;
			return false;
		}

		cout << "        PLY_reader: triangle mesh has:" << endl;
		cout << "            Vertices = " << nVertices << endl;
		cout << "            Faces = " << nFaces << endl;
		return true;
	}

	/* LOADING VERTICES AND MESHES */

	// ---------------- BINARY ----------------
	// ---- LITTLE ENDIAN 1.0
	void __load_vertices_binary_le_1_0(ifstream& fin, int n_verts, vector<float>& ply_verts) {
		float v1, v2, v3;

		for (int i = 0; i < n_verts; ++i) {
			fin.read((char *)&v1, sizeof(float));
			fin.read((char *)&v2, sizeof(float));
			fin.read((char *)&v3, sizeof(float));

			ply_verts.push_back(v1);
			ply_verts.push_back(v2);
			ply_verts.push_back(v3);
		}
	}

	void __load_faces_binary_le_1_0(ifstream& fin, int n_faces, vector<int>& ply_tris) {
		// indices of vertices per face
		int tri[3];
		// number of vertices per face
		unsigned char n_vertex_face;

		for (int i = 0; i < n_faces; ++i) {
			fin.read((char *)&n_vertex_face, sizeof(unsigned char));

			// read vertices indexes
			fin.read((char *)&tri[0], sizeof(int));
			fin.read((char *)&tri[1], sizeof(int));
			fin.read((char *)&tri[2], sizeof(int));
			// put the indices in the vector
			ply_tris.push_back(tri[0]);
			ply_tris.push_back(tri[1]);
			ply_tris.push_back(tri[2]);

			// in case the face has 4 or more vertices...
			for(; n_vertex_face > 3; --n_vertex_face) {
				tri[1] = tri[2];
				fin.read((char *)&tri[2], sizeof(int));
				ply_tris.push_back(tri[0]);
				ply_tris.push_back(tri[1]);
				ply_tris.push_back(tri[2]);
			}
		}
	}

	// ---------------- ASCII ----------------
	// ---- ASCII 1.0

	void __load_vertices_ascii_1_0(ifstream& fin, int n_verts, vector<float>& ply_verts) {
		float v1,v2,v3;

		for (int i = 0; i < n_verts; ++i) {
			fin >> v1 >> v2 >> v3;

			ply_verts.push_back(v1);
			ply_verts.push_back(v2);
			ply_verts.push_back(v3);
		}
	}

	void __load_faces_ascii_1_0(ifstream& fin, int n_faces, vector<int>& ply_tris) {
		// indices of vertices per face
		int tri[3];
		// number of vertices per face
		int n_vertex_face;

		for (int i = 0; i < n_faces; ++i) {
			fin >> n_vertex_face;

			// read vertices indexes
			fin >> tri[0] >> tri[1] >> tri[2];

			// put the indices in the vector
			ply_tris.push_back(tri[0]);
			ply_tris.push_back(tri[1]);
			ply_tris.push_back(tri[2]);

			// in case the face has 4 or more vertices...
			for(; n_vertex_face > 3; --n_vertex_face) {
				tri[1] = tri[2];
				fin >> tri[2];

				ply_tris.push_back(tri[0]);
				ply_tris.push_back(tri[1]);
				ply_tris.push_back(tri[2]);
			}
		}
	}

	void __add_model_to_mesh(const vector<float>& plyVertices, const vector<int>& plyTriangles, TriangleMesh& mesh) {
		// every position that is a multiple of 3 starts
		// a new vertex -> (0,1,2) are the coordinates of a vertex,
		// (3,4,5) are the coordinates of the next vertex, and so on.
		for (unsigned int i = 0; i < plyVertices.size(); i += 3) {
			mesh.addVertex(vec3(plyVertices[i], plyVertices[i+1], plyVertices[i+2]));
		}
		// just like with the vertices, every position that is a
		// multiple of 3 starts a new triangle.
		for (unsigned int i = 0; i < plyTriangles.size(); i += 3) {
			mesh.addTriangle(plyTriangles[i], plyTriangles[i+1], plyTriangles[i+2]);
		}
	}

	bool read_mesh(const string& filename, TriangleMesh& mesh) {
		ifstream fin;
		int n_verts, n_faces;

		cout << "    PLY_reader: opening file '" << filename << "'..." << endl;
		fin.open(filename.c_str(), ios_base::in | ios_base::binary);
		if (not fin.is_open()) {
			cerr << "    PLY_reader::read_mesh - Error:" << endl;
			cerr << "        Could not open file '" << filename << "'." << endl;
			return false;
		}

		cout << "    PLY_reader: reading header..." << endl;
		string format;
		bool header_read = __load_header(fin, n_verts, n_faces, format);
		if (not header_read) {
			fin.close();
			cerr << "    PLY_reader::read_mesh - Error:" << endl;
			cerr << "        Bad input file format." << endl;
			return false;
		}

		vector<float> plyVertices;
		vector<int> plyTriangles;

		// Load the vertices and the faces from the ply file.
		// Call the appropriate functions depending on the format.
		if (format == "binary_little_endian 1.0") {
			cout << "    PLY_reader: loading vertices..." << endl;
			__load_vertices_binary_le_1_0(fin, n_verts, plyVertices);
			cout << "    PLY_reader: loading faces..." << endl;
			__load_faces_binary_le_1_0(fin, n_faces, plyTriangles);
		}
		else if (format == "ascii 1.0") {
			cout << "    PLY_reader: loading vertices..." << endl;
			__load_vertices_ascii_1_0(fin, n_verts, plyVertices);
			cout << "    PLY_reader: loading faces..." << endl;
			__load_faces_ascii_1_0(fin, n_faces, plyTriangles);
		}

		fin.close();

		// build the TriangleMesh object
		cout << "    PLY_reader: building triangle mesh..." << endl;
		__add_model_to_mesh(plyVertices, plyTriangles, mesh);

		cout << "    PLY_reader: triangle mesh built succesfully." << endl;
		return true;
	}

} // -- namespace PLY_reader
