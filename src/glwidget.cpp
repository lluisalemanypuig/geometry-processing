#include "glwidget.hpp"

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;

// PRIVATE

void GLWidget::set_projection(float aspect) {
	QMatrix4x4 projectionMatrix;

	projectionMatrix.perspective(60.0f, aspect, 0.01f, 100.0f);

	program->bind();
	program->setUniformValue("projection", projectionMatrix);
	program->release();
}

void GLWidget::set_modelview() {
	QMatrix4x4 modelviewMatrix;

	modelviewMatrix.translate(0.0f, 0.0f, -distance);
	modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
	modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
	program->bind();
	program->setUniformValue("modelview", modelviewMatrix);
	program->setUniformValue("normal_matrix", modelviewMatrix.normalMatrix());
	program->release();
}

void GLWidget::make_colors_rainbow_gradient() {
	/*
	float m = curvature_values[0];
	float M = curvature_values[0];

	for (size_t i = 1; i < curvature_values.size(); ++i) {
		m = std::min(m, curvature_values[i]);
		M = std::max(M, curvature_values[i]);
	}

	for (float v : curvature_values) {
		float c = (v - m)/(M - m);
		vertex_color[i] = QVector4D(c, c, c, 1.0f);
	}
	*/

	/*
	int h = int(hue * 256 * 6);
	int x = h % 0x100;

	int r = 0, g = 0, b = 0;
	switch (h/256) {
		case 0: r = 255; g = x;       break;
		case 1: g = 255; r = 255 - x; break;
		case 2: g = 255; b = x;       break;
		case 3: b = 255; g = 255 - x; break;
		case 4: b = 255; r = x;       break;
		case 5: r = 255; b = 255 - x; break;
	}
	*/
}

// PROTECTED

void GLWidget::initializeGL() {
	initializeOpenGLFunctions();

	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shader_vert.vert");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shader_frag.frag");
	program->link();
	if (not program->isLinked()) {
		cerr << "GLWidget::initializeGL - Error:" << endl;
		cerr << "Shader program has not linked" << endl;
		cerr << endl;
		cerr << "Log: " << endl;
		cerr << endl;
		cerr << program->log().toStdString();
		QApplication::quit();
	}
	program->bind();

	mesh.buildCube();
	if (not mesh.init(program)) {
		cerr << "GLWidget::initializeGL - Error:" << endl;
		cerr << "    Could not initialise mesh cube." << endl;
		QApplication::quit();
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	set_projection(static_cast<float>(w/h));
	set_modelview();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->bind();
	program->setUniformValue("b_lighting", tri_fill);
	if (tri_fill) {
		program->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
	}
	else {
		// 'fill' the triangles with white
		program->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mesh.render(*this);

		// render mesh in wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		program->setUniformValue("color", QVector4D(0.05f, 0.05f, 0.15f, 1.0f));
	}

	mesh.render(*this);
	program->release();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	// Rotation
	if (event->buttons() & Qt::LeftButton) {
		angleX += rotationFactor*(event->y() - lastMousePos.y());
		angleX = std::max(-maxRotationCamera, std::min(angleX, maxRotationCamera));
		angleY += rotationFactor*(event->x() - lastMousePos.x());
	}
	// Zoom
	if (event->buttons() & Qt::RightButton) {
		distance += 0.01f*(event->y() - lastMousePos.y());
		distance = std::max(minDistanceCamera, std::min(distance, maxDistanceCamera));
	}

	lastMousePos = event->pos();

	makeCurrent();
	set_modelview();
	doneCurrent();
	update();
}

// PUBLIC

GLWidget::GLWidget(QWidget *parent)
	:
	QOpenGLWidget(parent),
	tri_fill(true),
	angleX(0.0f), angleY(0.0f), distance(2.0f)
{
	program = nullptr;
	show_curvatures = false;
	vertex_color = vector<QVector4D>(mesh.n_vertices());
}

GLWidget::~GLWidget() {
	if (program) {
		delete program;
	}
}

void GLWidget::load_mesh(const QString& filename) {
	mesh.destroy();

	cout << "GLWidget: reading mesh..." << endl;
	PLY_reader::read_mesh(filename, mesh);

	cout << "GLWidget: initialising mesh..." << endl;

	makeCurrent();
	if (not mesh.init(program)) {
		cerr << "GLWidget::load_mesh - Error:" << endl;
		cerr << "    Could not initialise mesh." << endl;
		QApplication::quit();
	}
	doneCurrent();
	update();

	mesh.make_neighbourhood_data();
}

void GLWidget::set_polygon_mode(bool bFill) {
	tri_fill = bFill;

	makeCurrent();
	if (tri_fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	doneCurrent();
	update();
}

void GLWidget::set_curvature_display(const curvature& _cd) {
	cd = _cd;

	if (cd == curvature::Gauss) {
		cout << "Gauss curvature" << endl;
		mesh.compute_Kg(curvature_values);
		show_curvatures = true;
	}
	else if (cd == curvature::Mean) {
		cout << "Mean curvature" << endl;
		mesh.compute_Kh(curvature_values);
		show_curvatures = true;
	}
	else if (cd == curvature::none) {
		cout << "No curvature to be displayed" << endl;
		curvature_values.clear();
		show_curvatures = false;
	}

	float cm = numeric_limits<float>::max();
	float cM = numeric_limits<float>::min();
	for (float v : curvature_values) {
		cm = std::min(cm, v);
		cM = std::max(cm, v);
	}
	float d = cM - cm;

	// copy the colors into a 'sorted' vector
	vector<vec3> colors(mesh.n_vertices());
	for (size_t i = 0; i < colors.size(); ++i) {
		float v = curvature_values[i];
		float c = v/d - cm/d;
		colors[i] = vec3(c,c,c);
	}

	makeCurrent();

	delete program;
	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex_col.vert");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/vertex_col.frag");
	program->link();
	if (not program->isLinked()) {
		cerr << "GLWidget::initializeGL - Error:" << endl;
		cerr << "Shader program has not linked" << endl;
		cerr << endl;
		cerr << "Log: " << endl;
		cerr << endl;
		cerr << program->log().toStdString();
		QApplication::quit();
	}

	mesh.init(program, colors);
	set_projection(static_cast<float>(this->width()/this->height()));
	set_modelview();
	doneCurrent();
	update();
}

