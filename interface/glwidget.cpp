#include "glwidget.hpp"

// C++ includes
#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

// Qt includes
#include <QApplication>

// algorithms includes
#include <geoproc/curvature/curvature.hpp>
#include <geoproc/ply_reader.hpp>

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;

inline
void make_colors_rainbow_gradient
(const vector<float>& values, curv_type ct, vector<vec3>& cols)
{
	// maximum value to determine the [min,max]
	// range of the mean curvature
	const float phi = 0.5;

	// minimum and maximum values of curvature
	float cm = numeric_limits<float>::max();
	float cM = -numeric_limits<float>::max();
	for (size_t i = 0; i < values.size(); ++i) {
		float v = values[i];
		if (ct == curv_type::Gauss) {
			if (v < -2*M_PI or v > 2*M_PI) {
				// if the curvature is outside [-2pi,2pi]
				// then discard it. The Gauss curvature
				// can't be greater than 2pi, though.
				continue;
			}
			cm = std::min(cm, v);
			cM = std::max(cM, v);
		}
		else if (ct == curv_type::Mean) {
			cm = std::min(cm, v);
			cM = phi;
		}
	}
	float d = cM - cm;
	cols = vector<vec3>(values.size());

	// ----------------------- //
	// Colour rainbow gradient //
	for (size_t i = 0; i < values.size(); ++i) {

		float r, g, b;
		if (values[i] < cm) {
			r = g = b = 0.0f;
		}
		else if (values[i] >= cM) {
			r = g = b = 1.0f;
		}
		else {
			float s = (values[i] - cm)/d;
			if (s <= 0.0f) {
				r = 0.0f;
				g = 0.0f;
				b = 0.0f;
			}
			else if (s <= 0.2f) {
				// RED
				// from 0.0 to 0.2
				r = 5.0f*s;
				g = 0.0f;
				b = 0.0f;
			}
			else if (s <= 0.4f) {
				// YELLOW
				// from 0.2 to 0.4
				r = 1.0f;
				g = 5.0f*s - 1.0f;
				b = 0.0f;
			}
			else if (s <= 0.6f) {
				// GREEN
				// from 0.4 to 0.6
				r = 5.0f*s - 2.0f;
				g = 1.0f;
				b = 0.0f;
			}
			else if (s <= 0.8f) {
				// TURQUOISE
				// from 0.6 to 0.8
				r = 0.0f;
				g = 1.0f;
				b = 5.0f*s - 3.0f;
			}
			else if (s <= 1.0f) {
				// BLUE
				// from 0.8 to 1.0
				r = 0.0f;
				g = 5.0f*s - 3.0f;
				b = 1.0f;
			}
			else if (1.0f <= s) {
				r = 0.0f;
				g = 0.0f;
				b = 1.0f;
			}
		}

		cols[i] = vec3(r,g,b);
	}
}

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

void GLWidget::delete_program() {
	if (program != nullptr) {
		delete program;
		program = nullptr;
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
}

void GLWidget::compute_curvature() {
	timing::time_point begin = timing::now();
	if (curv_display == curv_type::Gauss) {
		cout << "    Gauss ";
		algorithms::curvature::Gauss(mesh, curvature_values, nt);
	}
	else if (curv_display == curv_type::Mean) {
		cout << "    Mean ";
		algorithms::curvature::mean(mesh, curvature_values, nt);
	}
	timing::time_point end = timing::now();

	// output execution time
	cout << "curvature computed in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;
}

void GLWidget::show_curvature(bool should_load) {
	cout << "GLWidget::show_curvature - should the shader be loaded? "
		 << (should_load ? "Yes" : "No") << endl;

	if (should_load) {
		cout << "GLWidget::show_curvature - load curvature shader" << endl;

		makeCurrent();
		load_curvature_shader();
		set_projection();
		set_modelview();
		doneCurrent();
	}

	cout << "GLWidget::show_curvature - make colours with curvature" << endl;

	vector<vec3> colors;
	make_colors_rainbow_gradient(curvature_values, curv_display, colors);

	makeCurrent();
	mesh.init(program, colors);
	doneCurrent();
	update();
}

void GLWidget::init_mesh(bool make_neigh) {
	cout << "GLWidget::init_mesh - initialising mesh..." << endl;

	makeCurrent();
	if (not mesh.init(program)) {
		cerr << "GLWidget::init_mesh - Error:" << endl;
		cerr << "    Could not initialise mesh." << endl;
		QApplication::quit();
	}
	doneCurrent();
	update();

	cout << "GLWidget::init_mesh - corner edge data..." << endl;
	if (make_neigh) {
		mesh.make_neighbourhood_data();
	}

	if (curv_display != curv_type::none) {
		cout << "GLWidget::init_mesh - displaying curvature" << endl;
		cout << "    computing curvature..." << endl;
		compute_curvature();
		cout << "    displaying curvature..." << endl;
		show_curvature(false);
	}

	cout << "GLWidget::init_mesh - initialised succesfully!" << endl;
}

void GLWidget::initializeGL() {
	initializeOpenGLFunctions();

	load_simple_shader();

	program->bind();
	mesh.build_cube();
	mesh.scale_to_unit();
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
	makeCurrent();
	set_projection();
	set_modelview();
	doneCurrent();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->bind();
	if (pm == polymode::wireframe) {
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
		program->setUniformValue("wireframe", false);

		if (curv_display == curv_type::none) {
			// display with a flat color since there
			// is no curvature to be displayed
			program->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
			mesh.render(*this);
			program->release();
			return;
		}

		// display with curvature color
		mesh.render(*this);
		program->release();
		return;
	}

	cerr << "Error! nothing is displayed!" << endl;
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

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
	program = nullptr;

	pm = polymode::solid;
	change_curv_display = curv_type::none;
	curv_display = curv_type::none;

	angleX = 0.0f;
	angleY = 0.0f;
	distance = 2.0f;

	nt = 1;
}

GLWidget::~GLWidget() {
	delete_program();
}

void GLWidget::set_mesh(const RenderTriangleMesh& rm) {
	// free current mesh
	mesh.free_buffers();
	mesh.destroy();

	mesh = rm;

	init_mesh(false);
}

void GLWidget::load_mesh(const QString& filename) {
	mesh.free_buffers();
	mesh.destroy();

	cout << "GLWidget::load_mesh - reading mesh..." << endl;

	PLY_reader::read_mesh(filename.toStdString(), mesh);
	mesh.scale_to_unit();

	cout << "GLWidget::load_mesh - initialising mesh..." << endl;

	init_mesh(true);
}

void GLWidget::clear_mesh() {
	mesh.free_buffers();
	mesh.destroy();
	update();
}

void GLWidget::set_polygon_mode(const polymode& pmode) {
	pm = pmode;
}

void GLWidget::change_polygon_mode() {
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
			cerr << "    Unhandled selected polymode: expect undefined behaviour" << endl;
	}
	doneCurrent();
	update();
}

void GLWidget::set_curvature_display(const curv_type& cd) {
	change_curv_display = cd;
}

void GLWidget::change_curvature_display() {
	curvature_values.clear();

	if (change_curv_display == curv_type::none) {
		curv_display = curv_type::none;

		// free only the colour buffer
		mesh.free_buffers();

		makeCurrent();
		load_simple_shader();
		set_projection();
		set_modelview();
		mesh.init(program);
		doneCurrent();
		update();

		return;
	}

	// load program only if necessary: if a curvature
	// was already being shown and we are told to show
	// another curvature, do not load program. If, however,
	// we weren't showing any then load the program.
	bool load_shader = false;
	if (curv_display == curv_type::none) {
		load_shader = true;
	}

	curv_display = change_curv_display;

	compute_curvature();
	show_curvature(load_shader);
}

void GLWidget::set_num_threads(size_t _nt) {
	nt = _nt;
}

const RenderTriangleMesh& GLWidget::get_mesh() const {
	return mesh;
}
