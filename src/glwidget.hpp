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

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
	private:
		bool bPolygonFill;
		float angleX, angleY, distance;
		QPoint lastMousePos;

		QOpenGLShaderProgram *program;
		TriangleMesh mesh;

	private:
		void setProjection(float aspect);
		void setModelview();

	protected:
		void initializeGL();
		void resizeGL(int w, int h);
		void paintGL();

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	public:
		GLWidget(QWidget *parent);
		~GLWidget();

		void loadMesh(const QString& filename);
		void setPolygonMode(bool bFill);

};

