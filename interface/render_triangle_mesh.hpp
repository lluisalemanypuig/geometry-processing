#pragma once

// C includes
#include <assert.h>

// C++ includes
#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// Qt includes
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

// glm includes
#include <glm/glm.hpp>
using namespace glm;

// algorithms includes
#include <algorithms/triangle_mesh.hpp>

class RenderTriangleMesh : public TriangleMesh {
	private:
		/// Vertex array object for ...??
		QOpenGLVertexArrayObject vao;
		/// Vertex buffer object for the vertices.
		QOpenGLBuffer vbo_vertices;
		/// Vertex buffer object for the normals.
		QOpenGLBuffer vbo_normals;
		/// Vertex buffer object for the colors.
		QOpenGLBuffer vbo_colors;
		/// Vertex buffer object for the triangles.
		QOpenGLBuffer vbo_triangles;

	private:
		/**
		 * @brief Builds the necessary information for the vertex array/buffer objects.
		 *
		 * This is used to rearrange the vertices so that the first vertex
		 * of the first triangle is at the first position of the vector.
		 * @param[out] copy_verts A copy of the vector @ref vertices.\n
		 * This parameter will contain the same vertices as @ref vertices but
		 * in the same order they appear in the vector @ref triangles.\n
		 * This vector will be as large as @ref vertices.
		 * @param[out] normals The normals of each of the triangles in the mesh.\n
		 * For each triangle, the normal is repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param[out] perFaceTriangles I DO NOT KNOW!!! (YET)
		 */
		void make_VBO_data(
			vector<vec3>& copied_vertices,
			vector<vec3>& normals,
			vector<unsigned int>& perFaceTriangles
		);
		/**
		 * @brief Builds the necessary information for the vertex array/buffer objects.
		 *
		 * This is used to rearrange the vertices so that the first vertex
		 * of the first triangle is at the first position of the vector.
		 * @param[out] copy_verts A copy of the vector @ref vertices.\n
		 * This parameter will contain the same vertices as @ref vertices but
		 * in the same order they appear in the vector @ref triangles.\n
		 * This vector will be as large as @ref vertices.
		 * @param[out] normals The normals of each of the triangles in the mesh.\n
		 * For each triangle, the normal is repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param[out] perFaceTriangles I DO NOT KNOW!!! (YET)
		 */
		void make_VBO_data(
			const vector<vec3>& colors,
			vector<vec3>& copied_vertices,
			vector<vec3>& cols,
			vector<vec3>& normals,
			vector<unsigned int>& perFaceTriangles
		);

	public:
		/// Default constructor
		RenderTriangleMesh();
		/**
		 * @brief Copy constructor
		 *
		 * Buffers are not copied.
		 */
		RenderTriangleMesh(const RenderTriangleMesh& m);
		/// Destructor
		~RenderTriangleMesh();

		// OPERATORS

		RenderTriangleMesh& operator= (const RenderTriangleMesh& m);

		// INITIALISERS

		/// Generates the vertices and triangles of a cube.
		void build_cube();

		// MODIFIERS

		/**
		 * @brief Initialises the mesh with the shader @e program associated.
		 *
		 * Makes the data necessary to build the vertex buffer objects,
		 * loads the shader program (bind, ...).
		 * @param program An GLSL shader program.
		 * @return Returns false on error.
		 */
		bool init(QOpenGLShaderProgram *program);
		/**
		 * @brief Initialises the mesh with the shader @e program associated.
		 *
		 * Makes the data necessary to build the vertex buffer objects,
		 * loads the shader program (bind, ...).
		 * @param program An GLSL shader program.
		 * @param colors Specify color per vertex.
		 * @return Returns false on error.
		 */
		bool init(QOpenGLShaderProgram *program, const vector<vec3>& colors);
		/**
		 * @brief Frees all the memoery occupied by the mesh.
		 *
		 * Clears the contents of @ref vertices, @ref triangles,
		 * @ref opposite_corners, @ref corners and @ref boundary,
		 * and destroys the vertex array object @ref vao, and
		 * the vertex buffer objects @ref vbo_vertices, @ref vbo_normals,
		 * @ref vbo_triangles.
		 */
		void free_buffers();

		// OTHERS

		/**
		 * @brief Renders the mesh.
		 *
		 * Using the functions available in @e gl.
		 */
		void render(QOpenGLFunctions& gl);
};

