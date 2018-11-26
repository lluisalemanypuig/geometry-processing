#include "glwidget.hpp"

// C++ includes
#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

// Qt includes
#include <QApplication>

// geoproc includes
#include <geoproc/curvature/curvature.hpp>
#include <geoproc/ply_reader.hpp>

// custom includes
#include "utils.hpp"

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 0.25f;
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

void GLWidget::delete_program() {
	if (program != nullptr) {
		delete program;
		program = nullptr;
	}
}

void GLWidget::load_shader() {
	delete_program();
	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex_shader.vert");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment_shader.frag");
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

void GLWidget::compute_curvature() {
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	curvature_values.clear();

	timing::time_point begin = timing::now();
	if (current_curv_display == curv_type::Gauss) {
		cout << "    Gauss ";
		geoproc::curvature::Gauss(mesh, curvature_values, nt, &cm, &cM);
	}
	else if (current_curv_display == curv_type::Mean) {
		cout << "    Mean ";
		geoproc::curvature::mean(mesh, curvature_values, nt, &cm, &cM);
	}
	timing::time_point end = timing::now();

	// output execution time
	cout << "curvature computed in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;
}

void GLWidget::show_curvature(bool make_all_buffers) {
	cout << "GLWidget::show_curvature - should all buffers be made? "
		 << (make_all_buffers ? "Yes" : "No") << endl;

	cout << "GLWidget::show_curvature - make colours with curvature" << endl;

	vector<vec3> cols;
	float min, max;

	if (current_curv_display == curv_type::Gauss) {
		min_max::binning_around(curvature_values, 0.0, cm, cM, prop, min, max);
	}
	else {
		min_max::binning(curvature_values, cm, cM, prop, min, max);
	}

	coloring::colors_rainbow(curvature_values, min, max, cols);

	makeCurrent();
	if (make_all_buffers) {
		cout << "    GLWidget::show_curvature - make ALL buffers" << endl;
		mesh.init(program, cols);
	}
	else {
		cout << "    GLWidget::show_curvature - make only buffer colour" << endl;
		mesh.make_colours_buffer(program, cols);
	}
	doneCurrent();
	update();
}

void GLWidget::init_mesh(bool make_all_buffers) {
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

	if (current_curv_display != curv_type::none) {
		cout << "GLWidget::init_mesh - displaying curvature" << endl;
		cout << "    computing curvature..." << endl;
		compute_curvature();
		cout << "    displaying curvature..." << endl;
		show_curvature(make_all_buffers);
	}

	cout << "GLWidget::init_mesh - initialised succesfully!" << endl;
}

void GLWidget::initializeGL() {
	initializeOpenGLFunctions();

	load_shader();

	program->bind();
	mesh.build_cube();
	mesh.scale_to_unit();
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();
	bool init = mesh.init(program);
	if (not init) {
		cerr << "GLWidget::initializeGL (" << __LINE__ << ") - Error:" << endl;
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
		program->setUniformValue("curvature", false);
		program->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mesh.render(*this);

		// render triangle edges
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		program->setUniformValue("color", QVector4D(0.05f, 0.05f, 0.15f, 1.0f));
		if (curvature_values.size() == 0) {
			program->setUniformValue("curvature", false);
		}
		else {
			program->setUniformValue("curvature", true);
		}

		mesh.render(*this);
	}
	else if (pm == polymode::solid) {
		program->setUniformValue("wireframe", false);

		if (current_curv_display == curv_type::none) {
			// display with a flat color since there
			// is no curvature to be displayed
			program->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
			mesh.render(*this);
			program->release();
			return;
		}

		// display with curvature color
		program->setUniformValue("curvature", true);
		mesh.render(*this);
	}
	else if (pm == polymode::reflection_lines) {

	}
	else {
		// no polygon mode
		cerr << "GLWidget::paintGL (" << __LINE__ << ") - Warning:" << endl;
		cerr << "    no polygon mode selected" << endl;
	}

	program->release();
	return;
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
	to_curv_display = curv_type::none;
	current_curv_display = curv_type::none;

	angleX = 0.0f;
	angleY = 0.0f;
	distance = 2.0f;

	prop = 95.0f;
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

	init_mesh(true);
}

void GLWidget::load_mesh(const QString& filename) {
	mesh.free_buffers();
	mesh.destroy();

	cout << "GLWidget::load_mesh - reading mesh..." << endl;

	geoproc::PLY_reader::read_mesh(filename.toStdString(), mesh);
	mesh.make_normal_vectors();
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
	to_curv_display = cd;
}

void GLWidget::change_curvature_display() {
	// make all buffers only if the colour buffer
	// was not made before
	bool make_all_buffers = false;
	if (current_curv_display == curv_type::none) {
		make_all_buffers = true;
	}

	if (to_curv_display == curv_type::none) {
		current_curv_display = curv_type::none;

		curvature_values.clear();
		// free only the colour buffer
		mesh.free_buffers();

		makeCurrent();
		load_shader();
		set_projection();
		set_modelview();
		mesh.init(program);
		doneCurrent();
		update();

		return;
	}

	if (current_curv_display == to_curv_display) {
		// do not recompute...

		// ... but maybe remake colours
		if (to_prop != prop) {
			to_prop = prop;
			show_curvature(make_all_buffers);
		}
		return;
	}

	current_curv_display = to_curv_display;

	compute_curvature();
	show_curvature(make_all_buffers);
}

void GLWidget::change_display_curvature_proportion(float p) {
	prop = p;
	to_prop = p;

	if (current_curv_display == curv_type::none) {
		return;
	}
	show_curvature(false);
}

void GLWidget::change_curvature_proportion(float p) {
	to_prop = p;
}

void GLWidget::set_num_threads(size_t _nt) {
	nt = _nt;
}

const RenderTriangleMesh& GLWidget::get_mesh() const {
	return mesh;
}
