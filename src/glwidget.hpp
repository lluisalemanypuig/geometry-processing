#ifndef GLWIDGET_H
#define GLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "trianglemesh.hpp"

/*
 * Qt version in FIB:
 * /opt/Qt/5.9.6/gcc_64/bin/qmake
 *
 */

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
	GLWidget(QWidget *parent);
	~GLWidget();

	void loadMesh(const QString &filename);
	void setPolygonMode(bool bFill);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void setProjection(float aspect);
	void setModelview();

private:
	bool bPolygonFill;
	float angleX, angleY, distance;
	QPoint lastMousePos;

	QOpenGLShaderProgram *program;
	TriangleMesh mesh;

};

#endif // GLWIDGET_H
