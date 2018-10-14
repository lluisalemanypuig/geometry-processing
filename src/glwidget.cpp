#include "glwidget.hpp"

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;

// PRIVATE

void GLWidget::set_projection() {
	QMatrix4x4 projectionMatrix;

	float aspect = static_cast<float>(width())/height();
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

void GLWidget::make_colors_rainbow_gradient(const vector<float>& values, vector<vec3>& cols) {
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

	float cm = numeric_limits<float>::max();
	float cM = numeric_limits<float>::min();
	for (float v : values) {
		cm = std::min(cm, v);
		cM = std::max(cm, v);
	}
	float d = cM - cm;

	// copy the colors into a 'sorted' vector
	cols = vector<vec3>(mesh.n_vertices());
	for (size_t i = 0; i < cols.size(); ++i) {
		float v = curvature_values[i];
		float c = v/d - cm/d;
		cols[i] = vec3(c,c,c);
	}
}

void GLWidget::delete_program() {
	if (program != nullptr) {
		delete program;
	}
}

void GLWidget::load_simple_shader() {
	delete_program();
	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
	program->link();
	if (not program->isLinked()) {
		cerr << "GLWidget::initializeGL - Error:" << endl;
		cerr << "Shader program 'simple' has not linked" << endl;
		cerr << endl;
		cerr << "Log: " << endl;
		cerr << endl;
		cerr << program->log().toStdString();
		QApplication::quit();
	}
	set_projection();
	set_modelview();
}

void GLWidget::compute_curvature() {
	if (curv_display == curvature::Gauss) {
		mesh.compute_Kg(curvature_values);
	}
	else if (curv_display == curvature::Mean) {
		mesh.compute_Kh(curvature_values);
	}
}

void GLWidget::show_curvature(bool load_shader) {
	vector<vec3> colors;
	make_colors_rainbow_gradient(curvature_values, colors);
	makeCurrent();
	if (load_shader) {
		load_curvature_shader();
	}
	mesh.init(program, colors);
	doneCurrent();
	update();
}

void GLWidget::load_curvature_shader() {
	delete_program();
	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/curvature.vert");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/curvature.frag");
	program->link();
	if (not program->isLinked()) {
		cerr << "GLWidget::initializeGL - Error:" << endl;
		cerr << "Shader program 'curvature' has not linked" << endl;
		cerr << endl;
		cerr << "Log: " << endl;
		cerr << endl;
		cerr << program->log().toStdString();
		QApplication::quit();
	}
	set_projection();
	set_modelview();
}

// PROTECTED

void GLWidget::initializeGL() {
	initializeOpenGLFunctions();

	load_simple_shader();
	program->bind();
	mesh.buildCube();
	bool init = mesh.init(program);
	if (not init) {
		cerr << "GLWidget::initializeGL - Error:" << endl;
		cerr << "    Could not initialise mesh cube." << endl;
		QApplication::quit();
	}
	program->release();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	set_projection();
	set_modelview();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (pm == polymode::wireframe) {
		program->bind();

		program->setUniformValue("wireframe", true);

		// 'fill' the triangles with white
		program->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mesh.render(*this);

		// render triangle edges
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		program->setUniformValue("color", QVector4D(0.05f, 0.05f, 0.15f, 1.0f));

		mesh.render(*this);
		program->release();
		return;
	}

	if (pm == polymode::solid) {

		if (curv_display == curvature::none) {
			// display with a flat color since there
			// is no curvature to be displayed
			program->bind();
			program->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
			program->setUniformValue("wireframe", false);
			mesh.render(*this);
			program->release();
			return;
		}

		// display with curvature color
		program->bind();
		program->setUniformValue("wireframe", false);
		mesh.render(*this);
		program->release();
		return;
	}

	cerr << "Displaying nothing..." << endl;
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

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
	program = nullptr;
	vertex_color = vector<QVector4D>(mesh.n_vertices());

	pm = polymode::solid;
	curv_display = curvature::none;

	angleX = 0.0f;
	angleY = 0.0f;
	distance = 2.0f;
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

	if (curv_display != curvature::none) {
		cout << "Compute curvature" << endl;
		compute_curvature();
		show_curvature(false);
	}
}

void GLWidget::set_polygon_mode(const polymode& pmode) {
	pm = pmode;

	makeCurrent();
	switch (pm) {
		case polymode::solid:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			program->setUniformValue("wireframe", false);
			break;
		case polymode::wireframe:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			program->setUniformValue("wireframe", true);
			break;
		default:
			cerr << "GLWidget::set_polygon_mode - Warning:" << endl;
			cerr << "    No polymode selected: expect undefined behaviour" << endl;
	}
	doneCurrent();
	update();
}

void GLWidget::set_curvature_display(const curvature& cd) {
	// load program only if necessary
	bool load_shader = false;
	if (curv_display == curvature::none) {
		load_shader = true;
	}

	curv_display = cd;
	if (curv_display == curvature::none) {
		cout << "No curvature to be displayed" << endl;
		curvature_values.clear();
		makeCurrent();
		load_simple_shader();
		doneCurrent();
		update();
		return;
	}

	compute_curvature();
	show_curvature(load_shader);
}

