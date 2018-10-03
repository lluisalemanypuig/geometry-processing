#pragma once

// C++ includes
#include <iostream>
#include <vector>
using namespace std;

// Qt includes
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class TriangleMesh {
	private:
		vector<QVector3D> vertices;
		vector<int> triangles;

		QOpenGLVertexArrayObject vao;
		QOpenGLBuffer vboVertices, vboNormals, vboTriangles;

	private:
		void buildReplicatedVertices(
			vector<QVector3D>& replicatedVertices,
			vector<QVector3D>& normals,
			vector<unsigned int>& perFaceTriangles
		);
		void fillVBOs(
			vector<QVector3D>& replicatedVertices,
			vector<QVector3D>& normals,
			vector<unsigned int>& perFaceTriangles
		);

	public:
		TriangleMesh();
		~TriangleMesh();

		void addVertex(const QVector3D& position);
		void addTriangle(int v0, int v1, int v2);

		void buildCube();

		bool init(QOpenGLShaderProgram *program);
		void destroy();

		void render(QOpenGLFunctions& gl);
};

