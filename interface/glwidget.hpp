#pragma once

// C++ includes
#include <iostream>
#include <limits>
using namespace std;

// Qt includes
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>

// algorithms includes
#include <algorithms/curvature/curvature.hpp>
#include <algorithms/ply_reader.hpp>

// Custom includes
#include "render_triangle_mesh.hpp"

enum class polymode : int8_t {
	none = -1,
	solid,				// fill the triangles of the meshes
	wireframe,			// the meshes in wireframe mode
	reflection_lines	// render reflection lines on the mesh
};

enum class curvature : int8_t {
	none = -1,
	Gauss,	// display Kg = k1*k2
	Mean	// display Kh = (k1 + k2)/2
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
	private:
		/// Display the mesh in a certain polygon mode. See @ref polymode.
		polymode pm;
		/// Type of curvature to be displayed. See @ref curvature_display.
		curvature curv_display;

		float angleX, angleY, distance;
		QPoint lastMousePos;

		QOpenGLShaderProgram *program;
		RenderTriangleMesh mesh;

		/// Vertex buffer object for colors of each vertex.
		QOpenGLBuffer vbo_colors;

		/// Values of the curvature per vertex
		vector<float> curvature_values;
		vector<QVector4D> vertex_color;

	private:
		void set_projection();
		void set_modelview();

		void make_colors_rainbow_gradient(const vector<float>& v, vector<vec3>& cols);

		void delete_program();
		void load_simple_shader();
		void load_curvature_shader();

		void compute_curvature();
		void show_curvature(bool load_shader = false);

		void initializeGL();
		void resizeGL(int w, int h);
		void paintGL();

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	public:
		GLWidget(QWidget *parent);
		~GLWidget();

		void load_mesh(const QString& filename);

		/// Sets the polygon display mode. See @ref pm.
		void set_polygon_mode(const polymode& pm);
		/// Sets the type of curvature to be displayed. See @ref curvature_display.
		void set_curvature_display(const curvature& curv_display);
};

