#include "glwidget.hpp"

const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 3.0f;

// PRIVATE

void GLWidget::setProjection(float aspect) {
	QMatrix4x4 projectionMatrix;

	projectionMatrix.perspective(60, aspect, 0.01, 100.0);
	program->bind();
	program->setUniformValue("projection", projectionMatrix);
	program->release();
}

void GLWidget::setModelview() {
	QMatrix4x4 modelviewMatrix;

	modelviewMatrix.translate(0, 0, -distance);
	modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
	modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
	program->bind();
	program->setUniformValue("modelview", modelviewMatrix);
	program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
	program->release();
}

// PROTECTED

void GLWidget::initializeGL() {
	initializeOpenGLFunctions();

	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simpleshader.vert");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simpleshader.frag");
	program->link();
	if (!program->isLinked()) {
		cerr << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
		QApplication::quit();
	}
	program->bind();

	mesh.buildCube();
	if (!mesh.init(program)) {
		cerr << "Could not create vbo" << endl;
		QApplication::quit();
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	setProjection((float)w/h);
	setModelview();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program->bind();
	program->setUniformValue("bLighting", bPolygonFill);
	if (bPolygonFill) {
		program->setUniformValue("color", QVector4D(0.75, 0.8, 0.9, 1.0));
	}
	else {
		program->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//cube.render(*this);
		mesh.render(*this);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		program->setUniformValue("color", QVector4D(0.05, 0.05, 0.15, 1.0));
	}
	//cube.render(*this);
	mesh.render(*this);
	program->release();
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
	// Rotation
	if (event->buttons()&  Qt::LeftButton) {
		angleX += rotationFactor*(event->y() - lastMousePos.y());
		angleX = max(-maxRotationCamera, min(angleX, maxRotationCamera));
		angleY += rotationFactor*(event->x() - lastMousePos.x());
	}
	// Zoom
	if (event->buttons()&  Qt::RightButton) {
		distance += 0.01f*(event->y() - lastMousePos.y());
		distance = max(minDistanceCamera, min(distance, maxDistanceCamera));
	}

	lastMousePos = event->pos();

	makeCurrent();
	setModelview();
	doneCurrent();
	update();
}

// PUBLIC

GLWidget::GLWidget(QWidget *parent)
:
QOpenGLWidget(parent), bPolygonFill(true), angleX(0.0f),
angleY(0.0f), distance(2.0f)
{
	program = nullptr;
}

GLWidget::~GLWidget() {
	if (program) {
		delete program;
	}
}

void GLWidget::loadMesh(const QString& filename) {

	mesh.destroy();
	PLY_reader::read_mesh(filename, mesh);
	makeCurrent();
	if (!mesh.init(program)) {
		cerr << "Could not create vbo" << endl;
		QApplication::quit();
	}
	doneCurrent();
	update();
}

void GLWidget::setPolygonMode(bool bFill) {
	bPolygonFill = bFill;

	makeCurrent();
	if (bFill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	doneCurrent();
	update();
}
