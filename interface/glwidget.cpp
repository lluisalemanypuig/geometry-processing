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
#include <geoproc/parametrisation/parametrisation.hpp>
#include <geoproc/ply_reader.hpp>

// custom includes
#include "utils.hpp"

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 0.25f;
const float maxDistanceCamera = 3.0f;

#define line "[" << name << "] (" << __LINE__ << ")"
#define ERR "[" << name << "] Error (" << __LINE__ << "):"
#define WAR "[" << name << "] Warning (" << __LINE__ << "):"

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
	QMatrix4x4 modelview, modelview_inverse;

	modelview.translate(0.0f, 0.0f, -distance);
	modelview.rotate(angleX, 1.0f, 0.0f, 0.0f);
	modelview.rotate(angleY, 0.0f, 1.0f, 0.0f);
	modelview_inverse = modelview.inverted();

	program->bind();
	program->setUniformValue("modelview", modelview);
	program->setUniformValue("modelview_inverse", modelview_inverse);
	program->setUniformValue("normal_matrix", modelview.normalMatrix());
	program->setUniformValue("viewer_pos", modelview_inverse*QVector4D(0,0,0,1));
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
		cerr << "GLWidget::initializeGL " << line << " - Error:" << endl;
		cerr << "Shader program 'simple' has not linked" << endl;
		cerr << endl;
		cerr << "Log: " << endl;
		cerr << endl;
		cerr << program->log().toStdString();
		QApplication::quit();
	}
}

void GLWidget::compute_curvature() {
	cout << "GLWidget::compute_curvature " << line << " - computing curvature"
		 << endl;

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
	cout << "GLWidget::show_curvature " << line << " - should all buffers be made? "
		 << (make_all_buffers ? "Yes" : "No") << endl;

	cout << "GLWidget::show_curvature " << line << " - make colours with curvature" << endl;

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
		cout << "    GLWidget::show_curvature " << line << " - make ALL buffers" << endl;
		mesh.init(program, cols);
	}
	else {
		cout << "    GLWidget::show_curvature " << line << " - make only buffer colour" << endl;
		mesh.make_colours_buffer(program, cols);
	}
	program->bind();
	program->setUniformValue("curvature", true);
	program->release();
	doneCurrent();
	update();

	cout << "GLWidget::show_curvature " << line << " - curvature should be displayed" << endl;
}

void GLWidget::init_mesh(bool make_all_buffers) {
	cout << "GLWidget::init_mesh " << line << " - initialising mesh..." << endl;

	makeCurrent();
	if (not mesh.init(program)) {
		cerr << "GLWidget::init_mesh - Error:" << endl;
		cerr << "    Could not initialise mesh." << endl;
		QApplication::quit();
	}
	doneCurrent();
	update();

	cout << "GLWidget::init_mesh " << line << " - corner edge data..." << endl;

	if (current_curv_display != curv_type::none) {
		cout << "GLWidget::init_mesh " << line << ": displaying curvature" << endl;
		cout << "    computing curvature..." << endl;
		compute_curvature();
		cout << "    displaying curvature..." << endl;
		show_curvature(make_all_buffers);
	}

	cout << "GLWidget::init_mesh " << line << " - initialised succesfully!" << endl;
}

void GLWidget::initializeGL() {
	initializeOpenGLFunctions();
	load_shader();

	program->bind();
	program->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
	mesh.build_cube();
	mesh.scale_to_unit();
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();
	bool init = mesh.init(program);

	if (not init) {
		cerr << "GLWidget::initializeGL - " << ERR << endl;
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

	if (current_polymode == polymode::solid) {
		mesh.render(*this);
	}
	else if (current_polymode == polymode::wireframe) {
		/* Note to self: setting the uniforms here is necessary
		 * for a correct display of curvature values in wireframe
		 * mode, ...
		 */

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
		program->setUniformValue("curvature", curvature_values.size() > 0);

		mesh.render(*this);
	}
	else if (current_polymode == polymode::reflection_lines) {
		mesh.render(*this);
	}
	else if (current_polymode == polymode::harmonic_maps) {
		mesh.render(*this);
	}
	else {
		// no polygon mode
		cerr << "GLWidget::paintGL - " << WAR << endl;
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

	current_polymode = polymode::solid;
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

	cout << "GLWidget::load_mesh " << line << " - reading mesh..." << endl;

	geoproc::PLY_reader::read_mesh(filename.toStdString(), mesh);
	mesh.make_normal_vectors();
	mesh.scale_to_unit();

	cout << "GLWidget::load_mesh " << line << " - initialising mesh..." << endl;

	init_mesh(true);
}

void GLWidget::clear_mesh() {
	mesh.free_buffers();
	mesh.destroy();
	update();
}

void GLWidget::set_light_sources_refl_lines(int v) {
	makeCurrent();
	program->bind();
	program->setUniformValue("num_sources", v);
	program->release();
	doneCurrent();
	update();
}

void GLWidget::set_name(const string& s) {
	name = s;
}

void GLWidget::set_polygon_mode(const polymode& pmode) {
	to_polymode = pmode;
}

void GLWidget::change_polygon_mode() {
	cout << "GLWidget::change_polygon_mode " << line << endl;

	makeCurrent();
	program->bind();
	if (to_polymode == polymode::solid) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		program->setUniformValue("wireframe", false);
		program->setUniformValue("reflection_lines", false);
		program->setUniformValue("harmonic_maps", false);

		if (current_curv_display == curv_type::none) {
			program->setUniformValue(
				"color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f)
			);
		}
	}
	else if (to_polymode == polymode::wireframe) {
		// do not set glPolygonMode because we need two different
		// modes and they are set in the paintGL function.

		program->setUniformValue("wireframe", true);
		program->setUniformValue("reflection_lines", false);
		program->setUniformValue("harmonic_maps", false);
	}
	else if (to_polymode == polymode::reflection_lines) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		program->setUniformValue("curvature", false);
		program->setUniformValue("wireframe", false);
		program->setUniformValue("reflection_lines", true);
		program->setUniformValue("harmonic_maps", false);

		curvature_values.clear();
	}
	else if (to_polymode == polymode::harmonic_maps) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		program->setUniformValue("curvature", false);
		program->setUniformValue("wireframe", false);
		program->setUniformValue("reflection_lines", false);
		program->setUniformValue("harmonic_maps", true);
	}
	else {
		cerr << "GLWidget::set_polygon_mode - " << WAR << endl;
		cerr << "    Unhandled selected polymode: expect undefined behaviour" << endl;
	}

	if (to_polymode == polymode::solid or to_polymode == polymode::wireframe) {

		// we may need to display curvatures
		if (current_curv_display == curv_type::none) {
			// don't display curvature colours
			program->setUniformValue("curvature", false);
		}
		else {
			program->setUniformValue("curvature", true);
			// values may need to be recomputed
			change_curvature_display();
		}
	}

	current_polymode = to_polymode;

	program->release();
	doneCurrent();
	update();
}

void GLWidget::set_harmonic_map(const polymode& pmode) {
	assert(current_polymode == polymode::harmonic_maps);
	assert(pmode == polymode::harmonic_maps_Circle or
		   pmode == polymode::harmonic_maps_Square);

	if (pmode == current_polymode) {
		// nothing to do
		return;
	}

	smoothing::smooth_weight w = smoothing::smooth_weight::uniform;
	parametrisation::boundary_shape s;

	if (pmode == polymode::harmonic_maps_Circle) {
		s = parametrisation::boundary_shape::Circle;
	}
	else if (pmode == polymode::harmonic_maps_Square) {
		s = parametrisation::boundary_shape::Square;
	}

	mesh.make_neighbourhood_data();
	mesh.make_boundaries();

	if (mesh.get_boundaries().size() != 1) {
		cerr << "GLWidget::set_harmonic_map - " << WAR << endl;
		cerr << "    No boundaries in the mesh." << endl;
		cerr << "    Prevent the application from terminating by not calling" << endl;
		cerr << "    the algorithm." << endl;
		return;
	}

	vector<vec2> uvs;

	timing::time_point begin = timing::now();
	parametrisation::harmonic_maps(mesh, w, s, uvs);
	timing::time_point end = timing::now();

	cout << "Harmonic maps" << endl;
	cout << "    with shape: ";
	if (s == parametrisation::boundary_shape::Circle) {
		cout << "circle";
	}
	else if (s == parametrisation::boundary_shape::Square) {
		cout << "square";
	}
	cout << endl;
	cout << "    weight type: ";
	if (w == smoothing::smooth_weight::uniform) {
		cout << "uniform";
	}
	else if (w == smoothing::smooth_weight::cotangent) {
		cout << "cotangent";
	}
	cout << endl;
	cout << "    computed in " << timing::elapsed_seconds(begin,end)
		 << " s" << endl;

	cout << "Texture coordintes:" << endl;
	for (size_t i = 0; i < uvs.size(); ++i) {
		cout << "    " << i << ": " << uvs[i].x << "," << uvs[i].y << endl;
		cout << "        " << uvs[i].x + 1 << "," << uvs[i].y + 1 << endl;
	}

	makeCurrent();
	mesh.make_tex_coord_buffer(program, uvs);
	doneCurrent();
	update();
}

void GLWidget::set_curvature_display(const curv_type& cd) {
	to_curv_display = cd;
}

void GLWidget::change_curvature_display() {
	cout << "GLWidget::change_curvature_display " << line
		 << " - changing curvature display" << endl;

	// if we don't curvature colours anymore...
	if (to_curv_display == curv_type::none) {
		/* One of the followng:		none -> none
		 *							Gauss -> none
		 *							Mean -> none
		 * (current_curv_display -> to_curv_display)
		 */

		cout << "GLWidget::change_curvature_display " << line
			 << " - no curvature to show" << endl;

		current_curv_display = curv_type::none;
		curvature_values.clear();
		makeCurrent();
		program->bind();
		program->setUniformValue("curvature", false);
		program->setUniformValue(
			"color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f)
		);
		program->release();
		doneCurrent();
		update();
		return;
	}

	/* at this point we want to display curvature values,
	 * namely, the value 'to_curv_display' is different from
	 * 'curv_type::none'
	 */

	cout << "GLWidget::change_curvature_display " << line
		 << " - some curvature is to be displayed" << endl;

	// make all buffers only if the colour buffer was not made
	// before: if the current curvature mode is "deactivated"
	bool make_all_buffers = false;
	if (current_curv_display == curv_type::none) {
		make_all_buffers = true;

		cout << "GLWidget::change_curvature_display " << line
			 << " -     all bufferss will be made" << endl;
	}

	if (current_curv_display != to_curv_display) {
		/* One of the followng:		none -> Gauss,Mean
		 *							Gauss -> Mean
		 *							Mean -> Gauss
		 * (current_curv_display -> to_curv_display)
		 */

		cout << "GLWidget::change_curvature_display " << line
			 << " - clear the current curvature values" << endl;
		curvature_values.clear();
	}

	/* compute curvature values... */
	bool comp_curv = false;

	if (current_curv_display == to_curv_display) {
		/* One of the followng:		Gauss -> Gauss
		 *							Mean -> Mean
		 * (current_curv_display -> to_curv_display)
		 */

		// we may not need to recompute the values
		if (curvature_values.size() == 0) {
			// we do (maybe because the reflection
			// lines thing cleared them)
			comp_curv = true;
			make_all_buffers = true;

			cout << "GLWidget::change_curvature_display " << line
				 << " - recompute all the curvature and buffers" << endl;
		}
		else {
			// make only buffer colours
			make_all_buffers = false;

			cout << "GLWidget::change_curvature_display " << line
				 << " - do not make all buffers again" << endl;
		}
	}
	else {
		/* One of the followng:		None -> Gauss,Mean
		 *							Gauss -> Mean
		 *							Mean -> Gauss
		 * (current_curv_display -> to_curv_display)
		 */
		comp_curv = true;
	}

	if (comp_curv) {
		cout << "GLWidget::change_curvature_display " << line
			 << " - compute curvature..." << endl;

		current_curv_display = to_curv_display;
		compute_curvature();
	}

	// since there are already curvature values computed ...
	// make the colours

	cout << "GLWidget::change_curvature_display " << line
		 << " - show curvature" << endl;

	to_prop = prop;
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

const polymode& GLWidget::get_polygon_mode() const {
	return current_polymode;
}
const curv_type& GLWidget::get_curvature_display() const {
	return current_curv_display;
}
