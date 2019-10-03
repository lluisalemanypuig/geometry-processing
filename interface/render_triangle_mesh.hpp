/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

#pragma once

// C++ includes
#include <vector>
using namespace std;

// Qt includes
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

// glm includes
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
using namespace glm;

// algorithms includes
#include <geoproc/triangle_mesh.hpp>
using namespace geoproc;

class RenderTriangleMesh : public TriangleMesh {
	private:
		/// Vertex array object for ...??
		QOpenGLVertexArrayObject vao;
		/// Buffer object for the vertices.
		QOpenGLBuffer vbo_vertices;
		/// Buffer object for the normals.
		QOpenGLBuffer vbo_normals;
		/// Buffer object for the colors.
		QOpenGLBuffer vbo_colors;
		/// Buffer object for the texture coordinates.
		QOpenGLBuffer vbo_tex_coord;
		/// Buffer object for the triangles.
		QOpenGLBuffer vbo_triangles;

	private:

		/// Make the buffer objects
		bool make_buffers(
			QOpenGLShaderProgram *program,
			bool with_colours = false,
			bool with_tex = false
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
			vector<vec3d>& copied_vertices,
			vector<vec3d>& normals,
			vector<unsigned int>& perFaceTriangles
		);
		/**
		 * @brief Builds the necessary information for the vertex array/buffer objects.
		 *
		 * This is used to rearrange the vertices so that the first vertex
		 * of the first triangle is at the first position of the vector.
		 * @param[in] colors Color per vertex.
		 * @param[out] copy_verts A copy of the vector @ref vertices.\n
		 * This parameter will contain the same vertices as @ref vertices but
		 * in the same order they appear in the vector @ref triangles.\n
		 * This vector will be as large as @ref vertices.
		 * @param[out] cols The colors of each vertex of in the mesh.\n
		 * For each triangle, the colour is repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param[out] normals The normals of each of the triangles in the mesh.\n
		 * For each triangle, the normal is repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param[out] perFaceTriangles I DO NOT KNOW!!! (YET)
		 */
		void make_VBO_data(
			const vector<vec3d>& colors,
			vector<vec3d>& copied_vertices,
			vector<vec3d>& cols,
			vector<vec3d>& normals,
			vector<unsigned int>& perFaceTriangles
		);
		/**
		 * @brief Builds the necessary information for the vertex array/buffer objects.
		 *
		 * This is used to rearrange the vertices so that the first vertex
		 * of the first triangle is at the first position of the vector.
		 * @param[in] tex_coords Texture coordinates per vertex.
		 * @param[out] copy_verts A copy of the vector @ref vertices.\n
		 * This parameter will contain the same vertices as @ref vertices but
		 * in the same order they appear in the vector @ref triangles.\n
		 * This vector will be as large as @ref vertices.
		 * @param[out] texs The texture coordinate of each vertex of in the mesh.\n
		 * For each triangle, the coordaintes are repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param[out] normals The normals of each of the triangles in the mesh.\n
		 * For each triangle, the normal is repeated three times, so the
		 * final size of this vector is three times the number of triangles.
		 * @param[out] perFaceTriangles I DO NOT KNOW!!! (YET)
		 */
		void make_VBO_data(
			const vector<vec2d>& tex_coord,
			vector<vec3d>& copied_vertices,
			vector<vec2d>& texs,
			vector<vec3d>& normals,
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

		// modifierS

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
		bool init(QOpenGLShaderProgram *program, const vector<vec3d>& colors);
		/**
		 * @brief Initialises the mesh with the shader @e program associated.
		 *
		 * Makes the data necessary to build the vertex buffer objects,
		 * loads the shader program (bind, ...).
		 * @param program An GLSL shader program.
		 * @param tex_coord Specify texture coordinates per vertex.
		 * @return Returns false on error.
		 */
		bool init(QOpenGLShaderProgram *program, const vector<vec2d>& tex_coord);

		/**
		 * @brief Remakes the vertex buffer object.
		 * @pre The vertex buffer object must have been previously created.
		 * @return Returns false on error.
		 */
		bool make_vertices_buffers(QOpenGLShaderProgram *program);

		/**
		 * @brief Remakes the vertex and normal buffer objects.
		 * @pre Both vertex and normal buffer objects must have been
		 * previously created.
		 * @return Returns false on error.
		 */
		bool make_vertices_normals_buffers(QOpenGLShaderProgram *program);

		/**
		 * @brief Remakes the colour buffer object.
		 * @pre The colour buffer object must have been previously created.
		 * @return Returns false on error.
		 */
		bool make_colours_buffer(QOpenGLShaderProgram *program, const vector<vec3d>& colors);

		/**
		 * @brief Remakes the texture coordinate buffer object.
		 *
		 * If the buffer was not made then everything is remade.
		 * @return Returns false on error.
		 */
		bool make_tex_coord_buffer(QOpenGLShaderProgram *program, const vector<vec2d>& texs);

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

		/// Free the colour buffer.
		void free_colour_buffer();
		/// Free the colour buffer.
		void free_tex_coord_buffer();

		// OTHERS

		/**
		 * @brief Renders the mesh.
		 *
		 * Using the functions available in @e gl.
		 */
		void render(QOpenGLFunctions& gl);
};

