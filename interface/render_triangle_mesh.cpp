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

#include "render_triangle_mesh.hpp"

// C includes
#include <assert.h>

// C++ includes
#include <functional>
#include <iostream>
#include <cmath>

// Custom includes
#include "err_war_helper.hpp"

// LOCAL-DEFINED

// PRIVATE

bool RenderTriangleMesh::make_buffers
(QOpenGLShaderProgram *program, bool with_colours, bool with_tex)
{
	/* ----- VAO create ----- */
	vao.create();
	if (vao.isCreated()) {
		vao.bind();
	}
	else {
		cerr << ERR("TriangleMesh::make_buffers", "") << endl;
		cerr << "    Array object 'vao' not created." << endl;
		return false;
	}

	/* ----- VBO VERTICES create ----- */
	vbo_vertices.create();
	if (vbo_vertices.isCreated()) {
		if (not vbo_vertices.bind()) {
			cerr << ERR("TriangleMesh::make_buffers", "") << endl;
			cerr << "    Buffer object 'vbo_vertices' not bound" << endl;
			return false;
		}
	}
	else {
		cerr << ERR("TriangleMesh::make_buffers", "") << endl;
		cerr << "    Buffer object 'vbo_vertices' not created." << endl;
		return false;
	}
	vbo_vertices.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_DOUBLE, 0, 3, 0);
	vbo_vertices.release();

	/* ----- VBO NORMALS create ----- */
	vbo_normals.create();
	if (vbo_normals.isCreated()) {
		if (not vbo_normals.bind()) {
			cerr << ERR("TriangleMesh::make_buffers", "") << endl;
			cerr << "    Buffer object 'vbo_normals' not bound" << endl;
			return false;
		}
	}
	else {
		cerr << ERR("TriangleMesh::make_buffers", "") << endl;
		cerr << "    Buffer object 'vbo_normals' not created." << endl;
		return false;
	}
	vbo_normals.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_DOUBLE, 0, 3, 0);
	vbo_normals.release();

	/* ----- VBO COLORS create ----- */
	if (with_colours) {
		vbo_colors.create();
		if (vbo_colors.isCreated()) {
			if (not vbo_colors.bind()) {
				cerr << ERR("TriangleMesh::make_buffers", "") << endl;
				cerr << "    Buffer object 'vbo_colors' not bound" << endl;
				return false;
			}
		}
		else {
			cerr << ERR("TriangleMesh::make_buffers", "") << endl;
			cerr << "    Buffer object 'vbo_colors' not created." << endl;
			return false;
		}
		vbo_colors.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		program->enableAttributeArray(2);
		program->setAttributeBuffer(2, GL_DOUBLE, 0, 3, 0);
		vbo_colors.release();
	}

	/* ----- VBO TEXTURE COORDINATES create ----- */
	if (with_tex) {
		vbo_tex_coord.create();
		if (vbo_tex_coord.isCreated()) {
			if (not vbo_tex_coord.bind()) {
				cerr << ERR("TriangleMesh::make_buffers", "") << endl;
				cerr << "    Buffer object 'vbo_tex_coord' not bound" << endl;
				return false;
			}
		}
		else {
			cerr << ERR("TriangleMesh::make_buffers", "") << endl;
			cerr << "    Buffer object 'vbo_tex_coord' not created." << endl;
			return false;
		}
		vbo_tex_coord.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		program->enableAttributeArray(3);
		program->setAttributeBuffer(3, GL_DOUBLE, 0, 2, 0);
		vbo_tex_coord.release();
	}

	/* ----- VBO TRIANGLES create ----- */
	vbo_triangles.create();
	if (vbo_triangles.isCreated()) {
		if (not vbo_triangles.bind()) {
			cerr << ERR("TriangleMesh::make_buffers", "") << endl;
			cerr << "    Buffer object 'vbo_triangles' not bound" << endl;
			return false;
		}
	}
	else {
		cerr << ERR("TriangleMesh::make_buffers", "") << endl;
		cerr << "    Buffer object 'vbo_triangles' not created." << endl;
		return false;
	}
	vbo_triangles.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	vbo_triangles.release();

	return true;
}

void RenderTriangleMesh::make_VBO_data
(
	vector<vec3d>& copied_vertices,
	vector<vec3d>& normals,
	vector<uint>& perFaceTriangles
)
{
	copied_vertices.resize(triangles.size());
	normals.resize(triangles.size());
	perFaceTriangles.resize(triangles.size());

	for (uint i = 0; i < triangles.size(); i += 3) {
		copied_vertices[i    ] = vertices[triangles[i    ]];
		copied_vertices[i + 1] = vertices[triangles[i + 1]];
		copied_vertices[i + 2] = vertices[triangles[i + 2]];

		normals[i    ] = normal_vectors[i/3];
		normals[i + 1] = normal_vectors[i/3];
		normals[i + 2] = normal_vectors[i/3];

		perFaceTriangles[i    ] = i;
		perFaceTriangles[i + 1] = i + 1;
		perFaceTriangles[i + 2] = i + 2;
	}
}

void RenderTriangleMesh::make_VBO_data
(
	const vector<vec3d>& colors,
	vector<vec3d>& vert_info,
	vector<vec3d>& cols,
	vector<vec3d>& normals,
	vector<uint>& perFaceTriangles
)
{
	vert_info.resize(triangles.size());
	cols.resize(triangles.size());
	normals.resize(triangles.size());
	perFaceTriangles.resize(triangles.size());

	for (uint i = 0; i < triangles.size(); i += 3) {
		vert_info[i    ] = vertices[triangles[i    ]];
		vert_info[i + 1] = vertices[triangles[i + 1]];
		vert_info[i + 2] = vertices[triangles[i + 2]];

		cols[i    ] = colors[triangles[i    ]];
		cols[i + 1] = colors[triangles[i + 1]];
		cols[i + 2] = colors[triangles[i + 2]];

		normals[i    ] = normal_vectors[i/3];
		normals[i + 1] = normal_vectors[i/3];
		normals[i + 2] = normal_vectors[i/3];

		perFaceTriangles[i    ] = i;
		perFaceTriangles[i + 1] = i + 1;
		perFaceTriangles[i + 2] = i + 2;
	}
}

void RenderTriangleMesh::make_VBO_data(
	const vector<vec2d>& tex_coord,
	vector<vec3d>& vert_info,
	vector<vec2d>& texs,
	vector<vec3d>& normals,
	vector<uint>& perFaceTriangles
)
{
	vert_info.resize(triangles.size());
	texs.resize(triangles.size());
	normals.resize(triangles.size());
	perFaceTriangles.resize(triangles.size());

	for (uint i = 0; i < triangles.size(); i += 3) {
		vert_info[i    ] = vertices[triangles[i    ]];
		vert_info[i + 1] = vertices[triangles[i + 1]];
		vert_info[i + 2] = vertices[triangles[i + 2]];

		texs[i    ] = tex_coord[triangles[i    ]];
		texs[i + 1] = tex_coord[triangles[i + 1]];
		texs[i + 2] = tex_coord[triangles[i + 2]];

		normals[i    ] = normal_vectors[i/3];
		normals[i + 1] = normal_vectors[i/3];
		normals[i + 2] = normal_vectors[i/3];

		perFaceTriangles[i    ] = i;
		perFaceTriangles[i + 1] = i + 1;
		perFaceTriangles[i + 2] = i + 2;
	}
}

// PUBLIC

RenderTriangleMesh::RenderTriangleMesh()
	: TriangleMesh(),
	  vbo_vertices(QOpenGLBuffer::VertexBuffer),
	  vbo_normals(QOpenGLBuffer::VertexBuffer),
	  vbo_triangles(QOpenGLBuffer::IndexBuffer)
{

}

RenderTriangleMesh::RenderTriangleMesh(const RenderTriangleMesh& m)
	: TriangleMesh(m),
	  vbo_vertices(QOpenGLBuffer::VertexBuffer),
	  vbo_normals(QOpenGLBuffer::VertexBuffer),
	  vbo_triangles(QOpenGLBuffer::IndexBuffer)
{
	// nothing to copy...
	// buffers should be remade in init() method
}

RenderTriangleMesh::~RenderTriangleMesh() {
	destroy();
	free_buffers();
}

// OPERATORS

RenderTriangleMesh& RenderTriangleMesh::operator= (const RenderTriangleMesh& m) {
	free_buffers();
	copy_mesh(m);
	return *this;
}

// INITIALISERS

void RenderTriangleMesh::build_cube() {
	double vertices[] = {
		-1, -1, -1,
		-1,  1, -1,
		 1,  1, -1,
		 1,  1,  1,
		 1, -1,  1,
		 1, -1, -1,
		-1, -1,  1,
		-1,  1,  1
	};

	int faces[] = {
		0, 1, 2, 3, 4, 2,
		5, 2, 4, 4, 6, 5,
		6, 0, 5, 5, 0, 2,
		4, 3, 6, 3, 7, 6,
		1, 0, 7, 0, 6, 7,
		3, 2, 1, 1, 7, 3
	};

	set_vertices( vector<double>(&vertices[0], &vertices[24]) );
	set_triangles( vector<int>(&faces[0], &faces[36]) );
	make_normal_vectors();

	make_neighbourhood_data();
}

bool RenderTriangleMesh::init(QOpenGLShaderProgram *program) {
	vector<vec3d> vert_info, normals;
	vector<uint> perFaceTriangles;
	make_VBO_data(vert_info, normals, perFaceTriangles);

	/* destroy buffers */
	free_buffers();

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	bool i = make_buffers(program);
	if (not i) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Some buffer could not be made." << endl;
		return false;
	}

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	bool vertices_bind = vbo_vertices.bind();
	if (not vertices_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Vertices buffer object was not bound" << endl;
		return false;
	}
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(double)*vert_info.size());
	vbo_vertices.release();

	bool normals_bind = vbo_normals.bind();
	if (not normals_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Normals buffer object was not bound" << endl;
		return false;
	}
	vbo_normals.allocate(&normals[0], 3*sizeof(double)*normals.size());
	vbo_normals.release();

	bool triangles_bind = vbo_triangles.bind();
	if (not triangles_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Triangles buffer object was not bound" << endl;
		return false;
	}
	vbo_triangles.allocate(&perFaceTriangles[0], sizeof(uint)*perFaceTriangles.size());
	vbo_triangles.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

bool RenderTriangleMesh::init
(QOpenGLShaderProgram *program, const vector<vec3d>& colors)
{
	vector<vec3d> vert_info, cols, normals;
	vector<uint> perFaceTriangles;
	make_VBO_data(colors, vert_info, cols, normals, perFaceTriangles);

	/* --------------- */
	/* destroy buffers */
	free_buffers();

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	bool i = make_buffers(program, true);
	if (not i) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Some buffer could not be made." << endl;
		return false;
	}

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	bool vertices_bind = vbo_vertices.bind();
	if (not vertices_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Vertices buffer object was not bound" << endl;
		return false;
	}
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(double)*vert_info.size());
	vbo_vertices.release();

	bool normals_bind = vbo_normals.bind();
	if (not normals_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Normals buffer object was not bound" << endl;
		return false;
	}
	vbo_normals.allocate(&normals[0], 3*sizeof(double)*normals.size());
	vbo_normals.release();

	bool colors_bind = vbo_colors.bind();
	if (not colors_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Colours buffer object was not bound" << endl;
		return false;
	}
	vbo_colors.allocate(&cols[0], 3*sizeof(double)*cols.size());
	vbo_colors.release();

	bool triangles_bind = vbo_triangles.bind();
	if (not triangles_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Triangles buffer object was not bound" << endl;
		return false;
	}
	vbo_triangles.allocate(&perFaceTriangles[0], sizeof(uint)*perFaceTriangles.size());
	vbo_triangles.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

bool RenderTriangleMesh::init
(QOpenGLShaderProgram *program, const vector<vec2d>& tex_coord)
{
	vector<vec3d> vert_info, normals;
	vector<vec2d> texs;
	vector<uint> perFaceTriangles;
	make_VBO_data(tex_coord, vert_info, texs, normals, perFaceTriangles);

	/* --------------- */
	/* destroy buffers */
	free_buffers();

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	bool i = make_buffers(program, false, true);
	if (not i) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Some buffer could not be made." << endl;
		return false;
	}

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	// -- VERTICES --
	bool vertices_bind = vbo_vertices.bind();
	if (not vertices_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Vertices buffer object was not bound" << endl;
		return false;
	}
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(double)*vert_info.size());
	vbo_vertices.release();

	// -- NORMALS --
	bool normals_bind = vbo_normals.bind();
	if (not normals_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Normals buffer object was not bound" << endl;
		return false;
	}
	vbo_normals.allocate(&normals[0], 3*sizeof(double)*normals.size());
	vbo_normals.release();

	// -- TEXTURE COORDINATES --
	bool texs_bind = vbo_tex_coord.bind();
	if (not texs_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Texture coordinates buffer object was not bound" << endl;
		return false;
	}
	vbo_tex_coord.allocate(&texs[0], 2*sizeof(double)*texs.size());
	vbo_tex_coord.release();

	// -- INDICES --
	bool triangles_bind = vbo_triangles.bind();
	if (not triangles_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Triangles buffer object was not bound" << endl;
		return false;
	}
	vbo_triangles.allocate
	(&perFaceTriangles[0], sizeof(uint)*perFaceTriangles.size());
	vbo_triangles.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

bool RenderTriangleMesh::make_vertices_buffers(QOpenGLShaderProgram *program) {
	// make vertex information
	vector<vec3d> vert_info;
	vert_info.resize(triangles.size());

	for (uint t = 0; t < triangles.size(); t += 3) {
		vert_info[t    ] = vertices[triangles[t    ]];
		vert_info[t + 1] = vertices[triangles[t + 1]];
		vert_info[t + 2] = vertices[triangles[t + 2]];
	}

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::make_vertices_buffers", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	vao.bind();

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	bool vertices_bind = vbo_vertices.bind();
	if (not vertices_bind) {
		cerr << ERR("TriangleMesh::make_vertices_buffers", "") << endl;
		cerr << "    Vertices buffer object was not bound" << endl;
		return false;
	}
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(double)*vert_info.size());
	vbo_vertices.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

bool RenderTriangleMesh::make_vertices_normals_buffers
(QOpenGLShaderProgram *program)
{
	// make vertex information
	vector<vec3d> vert_info, normal_info;
	vert_info.resize(triangles.size());
	normal_info.resize(triangles.size());

	for (uint t = 0; t < triangles.size(); t += 3) {
		vert_info[t    ] = vertices[triangles[t    ]];
		vert_info[t + 1] = vertices[triangles[t + 1]];
		vert_info[t + 2] = vertices[triangles[t + 2]];

		normal_info[t    ] = normal_vectors[t/3];
		normal_info[t + 1] = normal_vectors[t/3];
		normal_info[t + 2] = normal_vectors[t/3];
	}

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::make_vertices_normals_buffers", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	vao.bind();

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	bool vertices_bind = vbo_vertices.bind();
	if (not vertices_bind) {
		cerr << ERR("TriangleMesh::make_vertices_normals_buffers", "") << endl;
		cerr << "    Vertices buffer object was not bound" << endl;
		return false;
	}
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(double)*vert_info.size());
	vbo_vertices.release();

	bool normals_bind = vbo_normals.bind();
	if (not normals_bind) {
		cerr << ERR("TriangleMesh::make_vertices_normals_buffers", "") << endl;
		cerr << "    Normals buffer object was not bound" << endl;
		return false;
	}
	vbo_normals.allocate(&normal_info[0], 3*sizeof(double)*normal_info.size());
	vbo_normals.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

bool RenderTriangleMesh::make_colours_buffer
(QOpenGLShaderProgram *program, const vector<vec3d>& colors)
{
	// make colour information
	vector<vec3d> cols(triangles.size());
	for (uint i = 0; i < triangles.size(); ++i) {
		cols[i] =   colors[triangles[i]];
	}

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::make_colours_buffer", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	vao.bind();

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	bool colors_bind = vbo_colors.bind();
	if (not colors_bind) {
		cerr << ERR("TriangleMesh::make_colours_buffer", "") << endl;
		cerr << "    Colours buffer object was not bound" << endl;
		return false;
	}
	vbo_colors.allocate(&cols[0], 3*sizeof(double)*cols.size());
	vbo_colors.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

bool RenderTriangleMesh::make_tex_coord_buffer
(QOpenGLShaderProgram *program, const vector<vec2d>& texs)
{
	if (not vbo_tex_coord.isCreated()) {
		cout << PROG("TriangleMesh::make_tex_coord_buffer", "",
					 "free and remake all buffers") << endl;

		free_buffers();
		bool i = make_buffers(program, true, true);
		if (not i) {
			cerr << ERR("TriangleMesh::make_tex_coord_buffer", "") << endl;
			cerr << "    Some buffer could not be made." << endl;
			return false;
		}
		return init(program, texs);
	}

	cout << PROG("TriangleMesh::make_tex_coord_buffer", "",
				 "Remake texture buffers") << endl;

	// make texture coordinates information
	vector<vec2> texs_coords(triangles.size());
	for (uint i = 0; i < triangles.size(); ++i) {
		texs_coords[i] = texs[triangles[i]];
	}

	/* --------------------------------------- */
	/* Create the vertex array/buffer objects. */

	bool program_bind = program->bind();
	if (not program_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Shader program was not bound" << endl;
		return false;
	}

	vao.bind();

	/* ------------------------------------- */
	/* Fill the vertex array/buffer objects. */

	bool texs_bind = vbo_tex_coord.bind();
	if (not texs_bind) {
		cerr << ERR("TriangleMesh::init", "") << endl;
		cerr << "    Texture coordinate buffer object was not bound" << endl;
		return false;
	}
	vbo_tex_coord.allocate(&texs_coords[0], 2*sizeof(double)*texs_coords.size());
	vbo_tex_coord.release();

	// ----- release
	vao.release();
	program->release();

	return true;
}

void RenderTriangleMesh::free_buffers() {
	vao.destroy();
	vbo_vertices.destroy();
	vbo_normals.destroy();
	vbo_triangles.destroy();
	free_colour_buffer();
	free_tex_coord_buffer();
}

void RenderTriangleMesh::free_colour_buffer() {
	vbo_colors.destroy();
}

void RenderTriangleMesh::free_tex_coord_buffer() {
	vbo_tex_coord.destroy();
}

// GETTERS

// OTHERS

void RenderTriangleMesh::render(QOpenGLFunctions& gl) {
	vao.bind();
	vbo_triangles.bind();
	gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	vao.release();
}



