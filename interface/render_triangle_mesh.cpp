#include "render_triangle_mesh.hpp"

// LOCAL-DEFINED

// PRIVATE

void RenderTriangleMesh::make_VBO_data
(
	vector<vec3>& copied_vertices,
	vector<vec3>& normals,
	vector<unsigned int>& perFaceTriangles
)
{
	copied_vertices.resize(triangles.size());
	normals.resize(triangles.size());
	perFaceTriangles.resize(triangles.size());

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		copied_vertices[i    ] = vertices[triangles[i    ]];
		copied_vertices[i + 1] = vertices[triangles[i + 1]];
		copied_vertices[i + 2] = vertices[triangles[i + 2]];

		vec3 N = glm::normalize(glm::cross(
			vertices[triangles[i + 1]] - vertices[triangles[i]],
			vertices[triangles[i + 2]] - vertices[triangles[i]]
		));

		normals[i    ] = N;
		normals[i + 1] = N;
		normals[i + 2] = N;

		perFaceTriangles[i    ] = i;
		perFaceTriangles[i + 1] = i + 1;
		perFaceTriangles[i + 2] = i + 2;
	}
}

void RenderTriangleMesh::make_VBO_data
(
	const vector<vec3>& colors,
	vector<vec3>& vert_info,
	vector<vec3>& cols,
	vector<vec3>& normals,
	vector<unsigned int>& perFaceTriangles
)
{
	vert_info.resize(triangles.size());
	cols.resize(triangles.size());
	normals.resize(triangles.size());
	perFaceTriangles.resize(triangles.size());

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		vert_info[i    ] = vertices[triangles[i    ]];
		vert_info[i + 1] = vertices[triangles[i + 1]];
		vert_info[i + 2] = vertices[triangles[i + 2]];
		cols[i    ] =   colors[triangles[i    ]];
		cols[i + 1] =   colors[triangles[i + 1]];
		cols[i + 2] =   colors[triangles[i + 2]];

		vec3 N = glm::normalize(glm::cross(
			vertices[triangles[i + 1]] - vertices[triangles[i]],
			vertices[triangles[i + 2]] - vertices[triangles[i]]
		));

		normals[i    ] = N;
		normals[i + 1] = N;
		normals[i + 2] = N;

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
{ }

RenderTriangleMesh::~RenderTriangleMesh() {
	destroy();
	free_buffers();
}

void RenderTriangleMesh::build_cube() {
	GLfloat vertices[] = {
		-1, -1, -1,
		-1,  1, -1,
		 1,  1, -1,
		 1,  1,  1,
		 1, -1,  1,
		 1, -1, -1,
		-1, -1,  1,
		-1,  1,  1
	};

	GLint faces[] = {
		0, 1, 2, 3, 4, 2,
		5, 2, 4, 4, 6, 5,
		6, 0, 5, 5, 0, 2,
		4, 3, 6, 3, 7, 6,
		1, 0, 7, 0, 6, 7,
		3, 2, 1, 1, 7, 3
	};

	for (int i = 0; i < 8; ++i) {
		add_vertex(0.5f * vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	}
	for (int i = 0; i < 12; ++i) {
		add_triangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
	}

	make_neighbourhood_data();
}

bool RenderTriangleMesh::init(QOpenGLShaderProgram *program) {
	vector<vec3> vert_info, normals;
	vector<unsigned int> perFaceTriangles;
	make_VBO_data(vert_info, normals, perFaceTriangles);

	/* ------------------ */
	/* Create the vertex array/buffer objects. */

	program->bind();

	/* ----- VAO create ----- */
	vao.destroy();
	vao.create();
	if (vao.isCreated()) {
		vao.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex array object 'vao' not created." << endl;
		return false;
	}

	/* ----- VBO VERTICES create ----- */
	vbo_vertices.destroy();
	vbo_vertices.create();
	if (vbo_vertices.isCreated()) {
		vbo_vertices.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vboVertices' not created." << endl;
		return false;
	}
	vbo_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	/* ----- VBO NORMALS create ----- */
	vbo_normals.destroy();
	vbo_normals.create();
	if (vbo_normals.isCreated()) {
		vbo_normals.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vboNormals' not created." << endl;
		return false;
	}
	vbo_normals.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	/* ----- VBO TRIANGLES create ----- */
	vbo_triangles.destroy();
	vbo_triangles.create();
	if (vbo_triangles.isCreated()) {
		vbo_triangles.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vboTriangles' not created." << endl;
		return false;
	}
	vbo_triangles.setUsagePattern(QOpenGLBuffer::StaticDraw);

	// fill the vertex buffer objects for correct display.
	vbo_vertices.bind();
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(float)*vert_info.size());
	vbo_vertices.release();

	vbo_normals.bind();
	vbo_normals.allocate(&normals[0], 3*sizeof(float)*normals.size());
	vbo_normals.release();

	vbo_triangles.bind();
	vbo_triangles.allocate(&perFaceTriangles[0], sizeof(unsigned int)*perFaceTriangles.size());
	vbo_triangles.release();

	vao.release();
	program->release();
	/* ----- VAO release ----- */

	return true;
}

bool RenderTriangleMesh::init(QOpenGLShaderProgram *program, const vector<vec3>& colors) {
	vector<vec3> vert_info, cols, normals;
	vector<unsigned int> perFaceTriangles;
	make_VBO_data(colors, vert_info, cols, normals, perFaceTriangles);

	/* ------------------ */
	/* Create the vertex array/buffer objects. */
	program->bind();

	/* ----- VAO create ----- */
	vao.destroy();
	vao.create();
	if (vao.isCreated()) {
		vao.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex array object 'vao' not created." << endl;
		return false;
	}

	/* ----- VBO VERTICES create ----- */
	vbo_vertices.destroy();
	vbo_vertices.create();
	if (vbo_vertices.isCreated()) {
		vbo_vertices.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vbo_vertices' not created." << endl;
		return false;
	}
	vbo_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	/* ----- VBO NORMALS create ----- */
	vbo_normals.destroy();
	vbo_normals.create();
	if (vbo_normals.isCreated()) {
		vbo_normals.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vbo_normals' not created." << endl;
		return false;
	}
	vbo_normals.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	/* ----- VBO COLORS create ----- */
	vbo_colors.destroy();
	vbo_colors.create();
	if (vbo_colors.isCreated()) {
		vbo_colors.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vbo_colors' not created." << endl;
		return false;
	}
	vbo_colors.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(2);
	program->setAttributeBuffer(2, GL_FLOAT, 0, 3, 0);

	/* ----- VBO TRIANGLES create ----- */
	vbo_triangles.destroy();
	vbo_triangles.create();
	if (vbo_triangles.isCreated()) {
		vbo_triangles.bind();
	}
	else {
		cerr << "    TriangleMesh::init - Error:" << endl;
		cerr << "        Vertex buffer object 'vbo_triangles' not created." << endl;
		return false;
	}
	vbo_triangles.setUsagePattern(QOpenGLBuffer::StaticDraw);

	/* ----- VBO fill ----- */
	vbo_vertices.bind();
	vbo_vertices.allocate(&vert_info[0], 3*sizeof(float)*vert_info.size());
	vbo_vertices.release();

	vbo_normals.bind();
	vbo_normals.allocate(&normals[0], 3*sizeof(float)*normals.size());
	vbo_normals.release();

	vbo_colors.bind();
	vbo_colors.allocate(&cols[0], 3*sizeof(float)*cols.size());
	vbo_colors.release();

	vbo_triangles.bind();
	vbo_triangles.allocate(&perFaceTriangles[0], sizeof(unsigned int)*perFaceTriangles.size());
	vbo_triangles.release();

	/* ----- VAO,SHADER release ----- */
	vao.release();
	program->release();

	return true;
}

void RenderTriangleMesh::free_buffers() {
	vao.destroy();
	vbo_vertices.destroy();
	vbo_normals.destroy();
	vbo_triangles.destroy();
}

// GETTERS

// OTHERS

void RenderTriangleMesh::render(QOpenGLFunctions& gl) {
	vao.bind();
	vbo_triangles.bind();
	gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	vao.release();
}



