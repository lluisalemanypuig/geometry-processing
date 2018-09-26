#include <iostream>
#include <vector>
#include "plyreader.h"


bool PLYReader::readMesh(const QString &filename, TriangleMesh &mesh)
{
	ifstream fin;
	int nVertices, nFaces;

	fin.open(filename.toStdString().c_str(), ios_base::in | ios_base::binary);
	if(!fin.is_open())
		return false;
	if(!loadHeader(fin, nVertices, nFaces))
	{
		fin.close();
		return false;
	}

	vector<float> plyVertices;
	vector<int> plyTriangles;

	loadVertices(fin, nVertices, plyVertices);
	loadFaces(fin, nFaces, plyTriangles);
	fin.close();

	rescaleModel(plyVertices);
	addModelToMesh(plyVertices, plyTriangles, mesh);

	return true;
}

bool PLYReader::loadHeader(ifstream &fin, int &nVertices, int &nFaces)
{
	char line[100];

	fin.getline(line, 100);
	if(strncmp(line, "ply", 3) != 0)
		return false;
	nVertices = 0;
	fin.getline(line, 100);
	while(strncmp(line, "end_header", 10) != 0)
	{
		if(strncmp(line, "element vertex", 14) == 0)
			nVertices = atoi(&line[15]);
		fin.getline(line, 100);
		if(strncmp(line, "element face", 12) == 0)
			nFaces = atoi(&line[13]);
	}
	if(nVertices <= 0)
		return false;
	cout << "Loading triangle mesh" << endl;
	cout << "\tVertices = " << nVertices << endl;
	cout << "\tFaces = " << nFaces << endl;
	cout << endl;

	return true;
}

void PLYReader::loadVertices(ifstream &fin, int nVertices, vector<float> &plyVertices)
{
	int i;
	float value;

	for(i=0; i<nVertices; i++)
	{
		fin.read((char *)&value, sizeof(float));
		plyVertices.push_back(value);
		fin.read((char *)&value, sizeof(float));
		plyVertices.push_back(value);
		fin.read((char *)&value, sizeof(float));
		plyVertices.push_back(value);
	}
}

void PLYReader::loadFaces(ifstream &fin, int nFaces, vector<int> &plyTriangles)
{
	int i, tri[3];
	unsigned char nVrtxPerFace;

	for(i=0; i<nFaces; i++)
	{
		fin.read((char *)&nVrtxPerFace, sizeof(unsigned char));
		fin.read((char *)&tri[0], sizeof(int));
		fin.read((char *)&tri[1], sizeof(int));
		fin.read((char *)&tri[2], sizeof(int));
		plyTriangles.push_back(tri[0]);
		plyTriangles.push_back(tri[1]);
		plyTriangles.push_back(tri[2]);
		for(; nVrtxPerFace>3; nVrtxPerFace--)
		{
			tri[1] = tri[2];
			fin.read((char *)&tri[2], sizeof(int));
			plyTriangles.push_back(tri[0]);
			plyTriangles.push_back(tri[1]);
			plyTriangles.push_back(tri[2]);
		}
	}
}

void PLYReader::rescaleModel(vector<float> &plyVertices)
{
	unsigned int i;
	QVector3D center(0.0f, 0.0f, 0.0f), size[2];

	size[0] = QVector3D(1e10, 1e10, 1e10);
	size[1] = QVector3D(-1e10, -1e10, -1e10);
	for(i=0; i<plyVertices.size(); i+=3)
	{
		center += QVector3D(plyVertices[i], plyVertices[i+1], plyVertices[i+2]);
		size[0][0] = min(size[0][0], plyVertices[i]);
		size[0][1] = min(size[0][1], plyVertices[i+1]);
		size[0][2] = min(size[0][2], plyVertices[i+2]);
		size[1][0] = max(size[1][0], plyVertices[i]);
		size[1][1] = max(size[1][1], plyVertices[i+1]);
		size[1][2] = max(size[1][2], plyVertices[i+2]);
	}
	center /= plyVertices.size() / 3;

	float largestSize = max(size[1][0] - size[0][0], max(size[1][1] - size[0][1], size[1][2] - size[0][2]));

	for(i=0; i<plyVertices.size(); i+=3)
	{
		plyVertices[i] = (plyVertices[i] - center[0]) / largestSize;
		plyVertices[i+1] = (plyVertices[i+1] - center[1]) / largestSize;
		plyVertices[i+2] = (plyVertices[i+2] - center[2]) / largestSize;
	}
}

void PLYReader::addModelToMesh(const vector<float> &plyVertices, const vector<int> &plyTriangles, TriangleMesh &mesh)
{
	unsigned int i;

	for(i=0; i<plyVertices.size(); i+=3)
		mesh.addVertex(QVector3D(plyVertices[i], plyVertices[i+1], plyVertices[i+2]));
	for(i=0; i<plyTriangles.size(); i+=3)
		mesh.addTriangle(plyTriangles[i], plyTriangles[i+1], plyTriangles[i+2]);
}

