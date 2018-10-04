#pragma once

#include <iostream>
using namespace std;

// Qt includes
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>

// Custom includes
#include "triangle_mesh.hpp"
#include "ply_reader.hpp"

enum class curvature : int8_t {
	none = -1,
	Gauss,	// display Kg = k1*k2
	Mean	// display Kh = (k1 + k2)/2
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
	private:
		/// Display the polygons filled (true), or wireframe (false)
		bool poly_fill;
		/// Type of curvature to be displayed. See @ref curvature_display.
		curvature cd;

		float angleX, angleY, distance;
		QPoint lastMousePos;

		QOpenGLShaderProgram *program;
		TriangleMesh mesh;

	private:
		void set_projection(float aspect);
		void set_modelview();

	protected:
		void initializeGL();
		void resizeGL(int w, int h);
		void paintGL();

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	public:
		GLWidget(QWidget *parent);
		~GLWidget();

		void load_mesh(const QString& filename);

		/// Sets the polygon display mode. See @ref poly_fill.
		void set_polygon_mode(bool fill);
		/// Sets the type of curvature to be displayed. See @ref curvature_display.
		void set_curvature_display(const curvature& cd);
};

