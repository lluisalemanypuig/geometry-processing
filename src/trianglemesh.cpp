#include "trianglemesh.hpp"

// LOCAL-DEFINED

inline int next(int corner) {
	return 3*(corner/3) + (corner + 1)%3;
}

inline int previous(int corner) {
	return 3*(corner/3) + (corner + 2)%3;
}

struct CornerEdge
{
	int vertexA, vertexB, corner;

	bool operator<(const CornerEdge& cEdge) {
		return (vertexA < cEdge.vertexA) || ((vertexA == cEdge.vertexA)& & (vertexB < cEdge.vertexB));
	}
	bool operator==(const CornerEdge& cEdge) {
		return (vertexA == cEdge.vertexA)& & (vertexB == cEdge.vertexB);
	}
};

// PRIVATE

void TriangleMesh::buildReplicatedVertices
(
	vector<QVector3D>& replicatedVertices,
	vector<QVector3D>& normals,
	vector<unsigned int>& perFaceTriangles
)
{
	normals.resize(triangles.size());

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		replicatedVertices.push_back(vertices[triangles[i]]);
		replicatedVertices.push_back(vertices[triangles[i+1]]);
		replicatedVertices.push_back(vertices[triangles[i+2]]);

		QVector3D N = QVector3D::crossProduct(
			vertices[triangles[i+1]] - vertices[triangles[i]],
			vertices[triangles[i+2]] - vertices[triangles[i]]
		);
		N.normalize();
		normals[i] = N;
		normals[i+1] = N;
		normals[i+2] = N;

		perFaceTriangles.push_back(perFaceTriangles.size());
		perFaceTriangles.push_back(perFaceTriangles.size());
		perFaceTriangles.push_back(perFaceTriangles.size());
	}
}

void TriangleMesh::fillVBOs
(
	vector<QVector3D>& replicatedVertices,
	vector<QVector3D>& normals,
	vector<unsigned int>& perFaceTriangles
)
{
	vboVertices.bind();
	vboVertices.allocate(&replicatedVertices[0], 3*sizeof(float)*replicatedVertices.size());
	vboVertices.release();

	vboNormals.bind();
	vboNormals.allocate(&normals[0], 3*sizeof(float)*normals.size());
	vboNormals.release();

	vboTriangles.bind();
	vboTriangles.allocate(&perFaceTriangles[0], sizeof(int)*perFaceTriangles.size());
	vboTriangles.release();
}

// PUBLIC

TriangleMesh::TriangleMesh()
: vboVertices(QOpenGLBuffer::VertexBuffer),
  vboNormals(QOpenGLBuffer::VertexBuffer),
  vboTriangles(QOpenGLBuffer::IndexBuffer)
{ }

TriangleMesh::~TriangleMesh() { }

void TriangleMesh::addVertex(const QVector3D& position) {
	vertices.push_back(position);
}

void TriangleMesh::addTriangle(int v0, int v1, int v2) {
	triangles.push_back(v0);
	triangles.push_back(v1);
	triangles.push_back(v2);
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
	vector<QVector3D> replicatedVertices, normals;
	vector<unsigned int> perFaceTriangles;

	buildReplicatedVertices(replicatedVertices, normals, perFaceTriangles);

	program->bind();

	vao.destroy();
	vao.create();
	if (vao.isCreated()) {
		vao.bind();
	}
	else {
		cerr << "TriangleMesh::init - Error:" << endl;
		cerr << "    vertex array object 'vao' not created" << endl;
		return false;
	}

	vboVertices.destroy();
	vboVertices.create();
	if (vboVertices.isCreated()) {
		vboVertices.bind();
	}
	else {
		cerr << "TriangleMesh::init - Error:" << endl;
		cerr << "    vertex buffer object 'vboVertices' not created" << endl;
		return false;
	}
	vboVertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	vboNormals.destroy();
	vboNormals.create();
	if (vboNormals.isCreated()) {
		vboNormals.bind();
	}
	else {
		cerr << "TriangleMesh::init - Error:" << endl;
		cerr << "    vertex buffer object 'vboNormals' not created" << endl;
		return false;
	}
	vboNormals.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	vboTriangles.destroy();
	vboTriangles.create();
	if (vboTriangles.isCreated()) {
		vboTriangles.bind();
	}
	else {
		cerr << "TriangleMesh::init - Error:" << endl;
		cerr << "    vertex buffer object 'vboTriangles' not created" << endl;
		return false;
	}
	vboTriangles.setUsagePattern(QOpenGLBuffer::StaticDraw);

	fillVBOs(replicatedVertices, normals, perFaceTriangles);

	vao.release();
	program->release();

	return true;
}

void TriangleMesh::destroy() {
	vao.destroy();
	vboVertices.destroy();
	vboNormals.destroy();
	vboTriangles.destroy();

	vertices.clear();
	triangles.clear();
}

void TriangleMesh::render(QOpenGLFunctions& gl) {
	vao.bind();
	vboTriangles.bind();
	gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	vao.release();
}



