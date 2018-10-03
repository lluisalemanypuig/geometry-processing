#include "ply_reader.hpp"

namespace PLY_reader {

	bool __load_header(ifstream& fin, int& nVertices, int& nFaces) {
		char line[100];

		fin.getline(line, 100);
		if (strncmp(line, "ply", 3) != 0) {
			cerr << "PLY_reader::__loadHeader - Error:" << endl;
			cerr << "    Wrong format of file: first line does not contain 'ply'." << endl;
			return false;
		}
		nVertices = 0;
		fin.getline(line, 100);
		while (strncmp(line, "end_header", 10) != 0) {
			if (strncmp(line, "element vertex", 14) == 0) {
				nVertices = atoi(&line[15]);
			}
			fin.getline(line, 100);
			if (strncmp(line, "element face", 12) == 0) {
				nFaces = atoi(&line[13]);
			}
		}
		if (nVertices <= 0) {
			cerr << "PLY_reader::__loadHeader - Error:" << endl;
			cerr << "    Number of vertices read is negative." << endl;
			return false;
		}

		cout << "Loading triangle mesh" << endl;
		cout << "\tVertices = " << nVertices << endl;
		cout << "\tFaces = " << nFaces << endl;
		cout << endl;

		return true;
	}

	void __load_vertices(ifstream& fin, int nVertices, vector<float>& plyVertices) {
		float value;

		for (int i = 0; i < nVertices; ++i) {
			fin.read((char *)&value, sizeof(float));
			plyVertices.push_back(value);
			fin.read((char *)&value, sizeof(float));
			plyVertices.push_back(value);
			fin.read((char *)&value, sizeof(float));
			plyVertices.push_back(value);
		}
	}

	void __load_faces(ifstream& fin, int nFaces, vector<int>& plyTriangles) {
		int tri[3];
		unsigned char nVrtxPerFace;

		for (int i = 0; i < nFaces; ++i) {
			fin.read((char *)&nVrtxPerFace, sizeof(unsigned char));
			fin.read((char *)&tri[0], sizeof(int));
			fin.read((char *)&tri[1], sizeof(int));
			fin.read((char *)&tri[2], sizeof(int));
			plyTriangles.push_back(tri[0]);
			plyTriangles.push_back(tri[1]);
			plyTriangles.push_back(tri[2]);
			for(; nVrtxPerFace > 3; --nVrtxPerFace) {
				tri[1] = tri[2];
				fin.read((char *)&tri[2], sizeof(int));
				plyTriangles.push_back(tri[0]);
				plyTriangles.push_back(tri[1]);
				plyTriangles.push_back(tri[2]);
			}
		}
	}

	void __rescale_model(vector<float>& plyVertices) {
		QVector3D center(0.0f, 0.0f, 0.0f);

		QVector3D size[2];
		size[0] = QVector3D(1e10, 1e10, 1e10);
		size[1] = QVector3D(-1e10, -1e10, -1e10);

		for (unsigned int i = 0; i < plyVertices.size(); i += 3) {
			center += QVector3D(plyVertices[i], plyVertices[i+1], plyVertices[i+2]);
			size[0][0] = min(size[0][0], plyVertices[i]);
			size[0][1] = min(size[0][1], plyVertices[i+1]);
			size[0][2] = min(size[0][2], plyVertices[i+2]);
			size[1][0] = max(size[1][0], plyVertices[i]);
			size[1][1] = max(size[1][1], plyVertices[i+1]);
			size[1][2] = max(size[1][2], plyVertices[i+2]);
		}
		center /= plyVertices.size()/3;

		float largestSize = max(
			size[1][0] - size[0][0],
			max(
				size[1][1] - size[0][1],
				size[1][2] - size[0][2]
			)
		);

		for (unsigned int i = 0; i < plyVertices.size(); i += 3) {
			plyVertices[i] = (plyVertices[i] - center[0])/largestSize;
			plyVertices[i+1] = (plyVertices[i+1] - center[1])/largestSize;
			plyVertices[i+2] = (plyVertices[i+2] - center[2])/largestSize;
		}
	}

	void __add_model_to_mesh(const vector<float>& plyVertices, const vector<int>& plyTriangles, TriangleMesh& mesh) {
		// every position that is a multiple of 3 starts
		// a new vertex -> (0,1,2) are the coordinates of a vertex,
		// (3,4,5) are the coordinates of the next vertex, and so on.
		for (unsigned int i = 0; i < plyVertices.size(); i += 3) {
			mesh.addVertex(QVector3D(plyVertices[i], plyVertices[i+1], plyVertices[i+2]));
		}
		// just like with the vertices, every position that is a
		// multiple of 3 starts a new triangle.
		for (unsigned int i = 0; i < plyTriangles.size(); i += 3) {
			mesh.addTriangle(plyTriangles[i], plyTriangles[i+1], plyTriangles[i+2]);
		}
	}

	bool read_mesh(const QString& filename, TriangleMesh& mesh) {
		ifstream fin;
		int nVertices, nFaces;

		fin.open(filename.toStdString().c_str(), ios_base::in | ios_base::binary);
		if (not fin.is_open()) {
			cerr << "PLY_reader::read_mesh - Error:" << endl;
			cerr << "    Could not open file '" << filename.toStdString() << "'." << endl;
			return false;
		}
		if (not __load_header(fin, nVertices, nFaces)) {
			fin.close();
			cerr << "PLY_reader::read_mesh - Error:" << endl;
			cerr << "    Bad input file format." << endl;
			return false;
		}

		vector<float> plyVertices;
		vector<int> plyTriangles;

		// load the vertices and the faces from the ply file
		__load_vertices(fin, nVertices, plyVertices);
		__load_faces(fin, nFaces, plyTriangles);
		fin.close();

		// the model needs reescaling
		__rescale_model(plyVertices);

		// build the TriangleMesh object
		__add_model_to_mesh(plyVertices, plyTriangles, mesh);

		// the triangular mesh has now the vertices
		// and faces loaded inside.

		return true;
	}

} // -- namespace PLY_reader
