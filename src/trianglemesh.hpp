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
		/// The set of vertices of the mesh.
		vector<QVector3D> vertices;
		/**
		 * @brief The set of triangles of the mesh.
		 *
		 * Every three consecutive values starting at a
		 * position multiple of 3 contain the indexes of
		 * the vertices (stored in @ref vertices) of the
		 * triangle.
		 */
		vector<int> triangles;

		/// Vertex array object for ...??
		QOpenGLVertexArrayObject vao;
		/// Vertex buffer object for the vertices.
		QOpenGLBuffer vboVertices;
		/// Vertex buffer object for the normals.
		QOpenGLBuffer vboNormals;
		/// Vertex buffer object for the triangles.
		QOpenGLBuffer vboTriangles;

	private:
		/**
		 * @brief Builds the necessary information for the vertex array/buffer objects.
		 *
		 * @param copy_verts[out] A copy of the vector @ref vertices.\n
		 * This parameter will contain the same vertices as @ref vertices but
		 * in the same order they appear in the vector @ref triangles.\n
		 * This vector will be as large as @ref vertices.
		 * @param normals[out] The normals of each of the triangles in the mesh.\n
		 * For each triangle, the normal is repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param perFaceTriangles[out] I DO NOT KNOW!!! (YET)
		 */
		void make_VBO_data(
			vector<QVector3D>& copied_vertices,
			vector<QVector3D>& normals,
			vector<unsigned int>& perFaceTriangles
		);
		/**
		 * @brief Fills the array/buffer objects associated to this mesh.
		 *
		 * These objects are @ref vao, @ref vboVertices, @ref vboNormals,
		 * and @ref vboTriangles;
		 *
		 * The parameters @e copied_vertices, @e normals, and @e perFaceTriangles
		 * are the result of calling @ref make_BVO_data.
		 */
		void fillVBOs(
			const vector<QVector3D>& copied_vertices,
			const vector<QVector3D>& normals,
			const vector<unsigned int>& perFaceTriangles
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

