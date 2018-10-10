#include "triangle_mesh.hpp"

// LOCAL-DEFINED

inline
int next(int corner) {
	return 3*(corner/3) + (corner + 1)%3;
}

inline
int previous(int corner) {
	return 3*(corner/3) + (corner + 2)%3;
}

template<typename T>
inline T cotan(const T& a) { return std::cos(a)/std::sin(a); }

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
	vector<glm::vec3>& copied_vertices,
	vector<glm::vec3>& normals,
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

		glm::vec3 N = glm::normalize(glm::cross(
			vertices[triangles[i+1]] - vertices[triangles[i]],
			vertices[triangles[i+2]] - vertices[triangles[i]]
		));

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
	const vector<glm::vec3>& copied_vertices,
	const vector<glm::vec3>& normals,
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

float TriangleMesh::triangle_area(int i, int j, int k) const {
	//cout << "        triangle area of (" << i << "," << j << "," << k << ")" << endl;
	glm::vec3 ij = vertices[j] - vertices[i];
	glm::vec3 ik = vertices[k] - vertices[i];
	glm::vec3 c = glm::cross(ij, ik);
	return glm::length(c)/2.0f;
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

void TriangleMesh::addVertex(const glm::vec3& position) {
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

	/* build data structure so that we have pairs of edges
	 * (an edge is a pair of vertex indices) each associated
	 * to a corner index:
	 * if we have, for an edge (i,j)
	 *     i j c
	 *     i j o
	 * then it means that corner 'c' has opposite 'o',
	 * and viceversa. Therefore:
	 *     opposite_cornes[o] = c
	 *     opposite_cornes[c] = o
	 */
	vector<CornerEdge> data;
	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		int v0 = i;
		int v1 = i + 1;
		int v2 = i + 2;

		CornerEdge e1, e2, e3;

		// edge v0,v1
		e1.set_indexes(triangles[v0], triangles[v1], v2);
		// edge v1,v2
		e2.set_indexes(triangles[v1], triangles[v2], v0);
		// edge v0,v2
		e3.set_indexes(triangles[v0], triangles[v2], v1);

		data.push_back(e1);
		data.push_back(e2);
		data.push_back(e3);
	}
	// sort the vector so that the edge pairs are consecutive
	sort(data.begin(), data.end());

	// For each corner edge, check that a twin exists.
	// If so, we have found a pair of opposite corners
	for (size_t i = 0; i < data.size(); ++i) {
		// take corner indices
		int vA = data[i].vertexA;
		int vB = data[i].vertexB;

		if (data[i + 1].vertexA == vA and data[i + 1].vertexB == vB) {
			// a twin exists
			int o = data[i].corner;
			int c = data[i + 1].corner;
			opposite_corners[o] = c;
			opposite_corners[c] = o;

			// go to the next pair.
			++i;
		}
		else {
			// this single CornerEdge contains
			// an edge of the boundary -> hard boundary
			boundary.push_back( make_pair(triangles[vA], triangles[vB]) );
		}
	}
	if (boundary.size() > 0) {
		cerr << "Warning: this mesh contains hard boundary(ies)" << endl;
		cerr << "    Computing Some curvatures may lead to a crash of the application" << endl;
	}

	#if defined (DEBUG)
	bool sane = true;
	size_t i = 0;
	while (i < opposite_corners.size() and sane) {
		if (opposite_corners[i] != -1) {
			int o = opposite_corners[i];
			int c = opposite_corners[o];
			/* we must have
			 *     opposite_corners[c] = o <-> opposite_corners[o] = c
			 */
			if ((int)(i) != c) {
				cerr << "         corner " << i << " has opposite " << o << endl;
				cerr << "however, corner " << o << " has opposite " << c << endl;
				sane = false;
			}
		}
		++i;
	}
	assert( sane );
	#endif
}

void TriangleMesh::buildCube() {
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

	GLuint faces[] = {
		0, 1, 2, 3, 4, 2,
		5, 2, 4, 4, 6, 5,
		6, 0, 5, 5, 0, 2,
		4, 3, 6, 3, 7, 6,
		1, 0, 7, 0, 6, 7,
		3, 2, 1, 1, 7, 3
	};

	for (int i = 0; i < 8; ++i) {
		addVertex(0.5f * glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	}
	for (int i = 0; i < 12; ++i) {
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
	}

	make_neighbourhood_data();
}

bool TriangleMesh::init(QOpenGLShaderProgram *program) {
	vector<glm::vec3> copied_vertices, normals;
	vector<unsigned int> perFaceTriangles;
	make_VBO_data(copied_vertices, normals, perFaceTriangles);

	/* ------------------ */
	/* Load the shader program.
	 * Create the vertex array/buffer objects.
	 */

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
	fillVBOs(copied_vertices, normals, perFaceTriangles);

	vao.release();
	program->release();
	/* ----- VAO release ----- */

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

// GETTERS

size_t TriangleMesh::n_vertices() const {
	return vertices.size();
}

size_t TriangleMesh::n_faces() const {
	return triangles.size()/3;
}

void TriangleMesh::compute_Kh(vector<float>& Kh) const {

	// process one-ring of vertices around corner c of vertex i
	auto process_one_ring =
	[this](int i, int c, glm::vec3& sum, float& area) -> int {
		glm::vec3 u, v;

		// take the previous and next corners
		int p_c = previous(c);
		int n_c = next(c);

		//cout << "    Previous of corner " << c << " is " << p_c << endl;
		//cout << "    Next of corner " << c << " is " << n_c << endl;

		// --- compute angle beta
		int vert__p_c = this->triangles[p_c];
		int vert__n_c = this->triangles[n_c];
		// from previous to i
		u = glm::normalize(vertices[i] - vertices[vert__n_c]);
		// from next to i
		v = glm::normalize(vertices[vert__p_c] - vertices[vert__n_c]);
		float beta = std::acos( glm::dot(u,v) );

		//cout << "    Angle beta: " << beta << endl;
		//cout << "        corners: (" << c << "," << n_c << "," << p_c << ")" << endl;

		// now take the corner opposite to corner pc...
		int o_n_c = this->opposite_corners[n_c];

		//cout << "    Corner opposite to corner " << n_c << " is " << o_n_c << endl;

		// watch out! a corner may not have opposite
		// -> hard boundary
		if (o_n_c == -1) {
			cerr << "TriangleMesh::compute_Kh: Error!" << endl;
			cerr << "    Found a hard boundary. Quitting traversal..." << endl;
			return -1;
		}

		// ... and the previous' opposite's previous and next corners
		int p_o_n_c = previous(o_n_c);
		int n_o_n_c = next(o_n_c);

		//cout << "    Previous of corner " << o_n_c << " is " << p_o_n_c << endl;
		//cout << "    Next of corner " << o_n_c << " is " << n_o_n_c << endl;

		// --- compute angle alpha
		int vert__o_n_c = this->triangles[o_n_c];
		int vert__p_o_n_c = this->triangles[p_o_n_c];
		// from previous to i
		u = glm::normalize(vertices[i] - vertices[vert__o_n_c]);
		// from next to i
		v = glm::normalize(vertices[vert__p_o_n_c] - vertices[vert__o_n_c]);
		float alpha = std::acos( glm::dot(u,v) );

		//cout << "    Angle alpha: " << alpha << endl;
		//cout << "        corners: (" << n_o_n_c << "," << o_n_c << "," << p_o_n_c << ")" << endl;

		/*
		 * Find vertex vj of the formula:
		 * if we are looking at vi, we need vj to be
		 * the adjacent vertex to vi at the other end
		 * of the edge that joins the triangles whose
		 * angles we are inspecting. These are, in corners:
		 * (c,n_c,p_c), (p_o_n_c,o_n_c,n_o_n_c)
		 *
		 *             (o_n_c)
		 *              /\
		 *             /  \
		 * (n_o_n_c)  /    \ (p_o_n_c)
		 *        vi < ---- > vj
		 *       (c)  \    /  (p_c)
		 *             \  /
		 *              \/
		 *              vk
		 *             (n_c)
		 */

		// cotangent weights
		float F = cotan(alpha) + cotan(beta);
		//cout << "    weight: " << F << endl;

		int j = triangles[p_c];
		/*
		cout << "        * Vertex opposite to vertex " << i << " is " << j << endl;
		cout << "        -> this one belongs to the 1-ring" << endl;
		*/

		// compute contribution to curvature
		sum += F*(vertices[j] - vertices[i]);

		// copmute area of triangle vi,vj,vk
		int k = triangles[n_c];
		area += triangle_area(i,j,k);

		//cout << "    triangle area: " << triangle_area(i,j,k) << endl;

		// Return the next corner.
		// This corner must correspond to vertex i.
		assert(triangles[n_o_n_c] == i);

		//cout << "    Vertex neighbour of " << i << " is " << triangles[n_c] << endl;
		return n_o_n_c;
	};

	Kh = vector<float>(vertices.size(), 0.0);

	for (uint i = 1; i < vertices.size(); ++i) {
		// take a starting corner for i-th vertex
		int v = corners[i];	// notice that triangles[v] equals i
		glm::vec3 curv_vec(0.0f,0.0f,0.0f);
		float area = 0.0;

		// n: next corner
		int n = process_one_ring(i, v, curv_vec, area);
		while (n != v and n != -1) {
			n = process_one_ring(i, n, curv_vec, area);
		}

		if (n == -1) {
			Kh[i] = -1.0f;
		}
		else {
			// area of the Voronoi area around i
			area /= 3.0f;
			curv_vec *= (1.0f/(2.0f*area));

			// if we traversal went OK keep computed value
			Kh[i] = (1/2.0f)*glm::length(curv_vec);
			if (Kh[i] < 0.0f) {
				cerr << "        Mesh has negative mean curvature at this vertex" << endl;
			}
		}
	}
}

void TriangleMesh::compute_Kg(vector<float>& Kg) const {
	// process one-ring of vertices around corner c of vertex i
	auto process_one_ring =
	[this](int i, int c, float& angles, float& area) -> int {
		// take the previous and next corners
		int p_c = previous(c);
		int n_c = next(c);

		// now take the corner opposite to corner n_c
		int o_n_c = this->opposite_corners[n_c];

		// watch out! a corner may not have opposite
		// -> hard boundary
		if (o_n_c == -1) {
			cerr << "TriangleMesh::compute_Kh: Error!" << endl;
			cerr << "    Found a hard boundary. Quitting traversal..." << endl;
			return -1;
		}

		// take the next corner of opposite of next of c
		int n_o_n_c = next(o_n_c);

		/*
		 * Find vertex vj of the formula:
		 * if we are looking at vi, we need vj to be
		 * the adjacent vertex to vi at the other end
		 * of the edge that joins the triangles whose
		 * angles we are inspecting. These are, in corners:
		 * (c,n_c,p_c), (p_o_n_c,o_n_c,n_o_n_c)
		 *
		 *             (o_n_c)
		 *              /\
		 *             /  \
		 * (n_o_n_c)  /    \ (p_o_n_c)
		 *        vi < ---- > vk
		 *       (c)  \    /  (p_c)
		 *             \  /
		 *              \/
		 *              vj
		 *             (n_c)
		 */

		int j = triangles[n_c];

		// compute area of triangle vi,vj,vk
		int k = triangles[p_c];
		area += triangle_area(i,j,k);

		glm::vec3 ij = vertices[j] - vertices[i];
		glm::vec3 ik = vertices[k] - vertices[i];
		ij = glm::normalize(ij);
		ik = glm::normalize(ik);
		float theta = std::acos( glm::dot(ij,ik) );
		angles += theta;

		// Return the next corner.
		// This corner must correspond to vertex i.
		assert(triangles[n_o_n_c] == i);

		return n_o_n_c;
	};

	Kg = vector<float>(vertices.size(), 0.0);

	for (uint i = 0; i < vertices.size(); ++i) {
		// take a starting corner for i-th vertex
		int v = corners[i];	// notice that triangles[v] equals i
		float angles = 0.0f;
		float area = 0.0;

		// n: next corner
		int n = process_one_ring(i, v, angles, area);
		while (n != v and n != -1) {
			n = process_one_ring(i, n, angles, area);
		}

		if (n != -1) {
			// area of the Voronoi area around i
			area /= 3.0f;

			// if we traversal went OK keep computed value
			Kg[i] = (1.0f/area)*(2*M_PI - angles);
		}
	}
}

// OTHERS

void TriangleMesh::render(QOpenGLFunctions& gl) {
	vao.bind();
	vbo_triangles.bind();
	gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
	vao.release();
}



