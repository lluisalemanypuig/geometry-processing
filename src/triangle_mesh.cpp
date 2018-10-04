#include "triangle_mesh.hpp"

// LOCAL-DEFINED

inline int next(int corner) {
	return 3*(corner/3) + (corner + 1)%3;
}

inline int previous(int corner) {
	return 3*(corner/3) + (corner + 2)%3;
}

struct CornerEdge {
	int vertexA, vertexB, corner;

	CornerEdge() : vertexA(-1), vertexB(-1), corner(-1) { }

	void set_indexes(int va, int vb, int c) {
		if (va < vb) {
			vertexA = va;
			vertexB = vb;
		}
		else {
			vertexA = vb;
			vertexB = va;
		}
		corner = c;
	}

	bool operator< (const CornerEdge& cEdge) const {
		return (vertexA < cEdge.vertexA) || ((vertexA == cEdge.vertexA) && (vertexB < cEdge.vertexB));
	}
	bool operator== (const CornerEdge& cEdge) const {
		return (vertexA == cEdge.vertexA) && (vertexB == cEdge.vertexB);
	}
};

// PRIVATE

void TriangleMesh::make_VBO_data
(
	vector<QVector3D>& copied_vertices,
	vector<QVector3D>& normals,
	vector<unsigned int>& perFaceTriangles
)
{
	normals.resize(triangles.size());
	copied_vertices.resize(3*triangles.size());
	perFaceTriangles.resize(3*triangles.size());

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		copied_vertices[i] = vertices[triangles[i]];
		copied_vertices[i+1] = vertices[triangles[i+1]];
		copied_vertices[i+2] = vertices[triangles[i+2]];

		QVector3D N = QVector3D::crossProduct(
			vertices[triangles[i+1]] - vertices[triangles[i]],
			vertices[triangles[i+2]] - vertices[triangles[i]]
		);

		N.normalize();
		normals[i] = N;
		normals[i+1] = N;
		normals[i+2] = N;

		perFaceTriangles[i] = i;
		perFaceTriangles[i+1] = i+1;
		perFaceTriangles[i+2] = i+2;
	}
}

void TriangleMesh::fillVBOs
(
	const vector<QVector3D>& copied_vertices,
	const vector<QVector3D>& normals,
	const vector<unsigned int>& perFaceTriangles
)
{
	vbo_vertices.bind();
	vbo_vertices.allocate(&copied_vertices[0], 3*sizeof(float)*copied_vertices.size());
	vbo_vertices.release();

	vbo_normals.bind();
	vbo_normals.allocate(&normals[0], 3*sizeof(float)*normals.size());
	vbo_normals.release();

	vbo_triangles.bind();
	vbo_triangles.allocate(&perFaceTriangles[0], sizeof(unsigned int)*perFaceTriangles.size());
	vbo_triangles.release();
}

// PUBLIC

TriangleMesh::TriangleMesh()
: vbo_vertices(QOpenGLBuffer::VertexBuffer),
  vbo_normals(QOpenGLBuffer::VertexBuffer),
  vbo_triangles(QOpenGLBuffer::IndexBuffer)
{ }

TriangleMesh::~TriangleMesh() {
	destroy();
}

void TriangleMesh::addVertex(const QVector3D& position) {
	vertices.push_back(position);
}

void TriangleMesh::addTriangle(int v0, int v1, int v2) {
	triangles.push_back(v0);
	triangles.push_back(v1);
	triangles.push_back(v2);
}

void TriangleMesh::make_neighbourhood_data() {
	// ------------------
	// build corner table
	corners.resize(vertices.size());

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		// triangle i has 3 corners:
		// -> i+0, i+1, i+2
		// each corner j is associated to a vertex
		// triangles[i+1]=j <-> "the i+1 corner of the
		// triangle has vertex j".

		int v0 = triangles[i];
		corners[v0] = i;

		int v1 = triangles[i + 1];
		corners[v1] = i + 1;

		int v2 = triangles[i + 2];
		corners[v2] = i + 2;
	}

	// ---------------------------
	// build opposite_corner table

	// opposite_corners is initialised with -1 because
	// such opposite may not exist.
	opposite_corners.resize(triangles.size(), -1);

	vector<CornerEdge> data;
	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		int v0 = triangles[i];
		int v1 = triangles[i+1];
		int v2 = triangles[i+2];

		CornerEdge e1, e2, e3;

		// edge v0,v1
		e1.set_indexes(v0, v1, v2);
		// edge v1,v2
		e2.set_indexes(v1, v2, v0);
		// edge v0,v2
		e3.set_indexes(v0, v2, v1);

		data.push_back(e1);
		data.push_back(e2);
		data.push_back(e3);
	}

	sort(data.begin(), data.end());
}

void TriangleMesh::buildCube() {
	GLfloat vertices[] = {
		-1, -1, -1,
		 1, -1, -1,
		 1,  1, -1,
		-1,  1, -1,
		-1, -1,  1,
		 1, -1,  1,
		 1,  1,  1,
		-1,  1,  1
	};

	GLuint faces[] = {
		3, 1, 0, 3, 2, 1,
		5, 6, 7, 4, 5, 7,
		7, 3, 0, 0, 4, 7,
		1, 2, 6, 6, 5, 1,
		0, 1, 4, 5, 4, 1,
		2, 3, 7, 7, 6, 2
	};

	for (int i = 0; i < 8; ++i) {
		addVertex(0.5f * QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	}
	for (int i = 0; i < 12; ++i) {
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
	}
}

bool TriangleMesh::init(QOpenGLShaderProgram *program) {
	vector<QVector3D> copied_vertices, normals;
	vector<unsigned int> perFaceTriangles;
	make_VBO_data(copied_vertices, normals, perFaceTriangles);

	/* ------------------ */
	/* Load the shader program.
	 * Create the vertex array/buffer objects.
	 */

	program->bind();

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

	/* ------------------ */

	// fill the vertex buffer objects for correct display.
	fillVBOs(copied_vertices, normals, perFaceTriangles);

	vao.release();
	program->release();
	return true;
}

void TriangleMesh::destroy() {
	vao.destroy();
	vbo_vertices.destroy();
	vbo_normals.destroy();
	vbo_triangles.destroy();

	vertices.clear();
	triangles.clear();
	opposite_corners.clear();
	corners.clear();
}

void TriangleMesh::render(QOpenGLFunctions& gl) {
	vao.bind();
	vbo_triangles.bind();
	gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	vao.release();
}



