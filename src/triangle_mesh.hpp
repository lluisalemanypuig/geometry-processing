#pragma once

// C includes
#include <assert.h>

// C++ includes
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
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

class TriangleMesh {
	private:
		/// The set of vertices of the mesh.
		vector<glm::vec3> vertices;
		/**
		 * @brief The set of triangles of the mesh.
		 *
		 * Every three consecutive values starting at a
		 * position multiple of 3 contain the indexes of
		 * the vertices (stored in @ref vertices) of the
		 * triangle.
		 *
		 * Then, the @e i-th triangle has vertices
		 * @ref triangles[i], @ref triangles[i+1], @ref triangles[i+2]
		 */
		vector<int> triangles;
		/**
		 * @brief The set of opposite corners.
		 *
		 * This table stores for every corner its opposite
		 * corner. Therefore, we can access the vertex opposite
		 * to another vertex (with respect to an edge).
		 *
		 * Then @ref opposite_corners[@e i] = @e j iff corner @e j
		 * is the opposite corner of @e i.
		 */
		vector<int> opposite_corners;
		/**
		 * @brief Corner table.
		 *
		 * Relates each vertex of the mesh to a corner.
		 *
		 * A vertex may belong to several corners (as many
		 * as triangles it is found in), but this table
		 * associates each vertex to a single corner index.
		 */
		vector<int> corners;
		/**
		 * @brief Hard boundary of the mesh.
		 *
		 * The hard boundary is composed by pairs of vertices
		 * (each identified with a vertex index).
		 *
		 * The hard is boundary is defined as those edges ({vi,vj})
		 * of those triangles ({vi,vj,vk}) such that vk has no
		 * opposite corner.
		 */
		vector<pair<int,int> > boundary;

		/// Vertex array object for ...??
		QOpenGLVertexArrayObject vao;
		/// Vertex buffer object for the vertices.
		QOpenGLBuffer vbo_vertices;
		/// Vertex buffer object for the normals.
		QOpenGLBuffer vbo_normals;
		/// Vertex buffer object for the triangles.
		QOpenGLBuffer vbo_triangles;

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
			vector<glm::vec3>& copied_vertices,
			vector<glm::vec3>& normals,
			vector<unsigned int>& perFaceTriangles
		);
		/**
		 * @brief Fills the array/buffer objects associated to this mesh.
		 *
		 * These objects are @ref vao, @ref vbo_vertices, @ref vbo_normals,
		 * and @ref vbo_triangles;
		 *
		 * The parameters @e copied_vertices, @e normals, and @e perFaceTriangles
		 * are the result of calling @ref make_BVO_data.
		 */
		void fillVBOs(
			const vector<glm::vec3>& copied_vertices,
			const vector<glm::vec3>& normals,
			const vector<unsigned int>& perFaceTriangles
		);

	public:
		/// Default constructor
		TriangleMesh();
		/// Destructor
		~TriangleMesh();

		/// Generates the vertices and triangles of a cube.
		void buildCube();

		// MODIFIERS

		/**
		 * @brief Adds a vertex to the mesh.
		 *
		 * A simple call to push_back of @ref vertices.
		 */
		void addVertex(const glm::vec3& position);
		/**
		 * @brief Adds a triangle to the mesh.
		 *
		 * Three calls to push_back of @ref triangles.
		 */
		void addTriangle(int v0, int v1, int v2);

		/**
		 * @brief Builds the necessary tables to iterate through
		 * the one-ring of a vertex, ....
		 *
		 * Builds @ref corners and @ref opposite_corner tables.
		 * Also, just for the sake of completeness, it computes
		 * the boundary of the mesh in @ref boundary.
		 *
		 * This function should be called only after all
		 * vertices and triangles have been added.
		 */
		void make_neighbourhood_data();

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
		 * @brief Frees all the memoery occupied by the mesh.
		 *
		 * Clears the contents of @ref vertices, @ref triangles
		 * and destroys the vertex array object @ref vao, and
		 * the vertex buffer objects @ref vbo_vertices, @ref vbo_normals,
		 * @ref vbo_triangles.
		 */
		void destroy();

		// GETTERS

		/// Returns the number of vertices.
		size_t n_vertices() const;

		/// Returns the number of faces.
		size_t n_faces() const;

		/**
		 * @brief Computes the Mean curvature for each vertex.
		 *
		 * Kh is approximated using a discretisation of the
		 * Laplace-beltrami operator.
		 * @param[out] Kh @e Kh[i] contains an approximation of
		 * the mean curvature at vertex i.
		 */
		void compute_Kh(vector<float>& Kh) const;

		// OTHERS

		/**
		 * @brief Renders the mesh.
		 *
		 * Using the functions available in @e gl.
		 */
		void render(QOpenGLFunctions& gl);
};

