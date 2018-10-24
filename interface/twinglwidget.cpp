#include "twinglwidget.hpp"

// PROTECTED

void TwinGLWidget::mousePressEvent(QMouseEvent *event) {
	GLWidget::mousePressEvent(event);
}

void TwinGLWidget::mouseMoveEvent(QMouseEvent *event) {
	GLWidget::mouseMoveEvent(event);
	twin->set_display(angleX, angleY, distance);
}

// PUBLIC

TwinGLWidget::TwinGLWidget(QWidget *parent) : GLWidget(parent) {
	op = smoothing_operator::Laplacian;
	twin = nullptr;
}

TwinGLWidget::~TwinGLWidget() {

}

// SETTERS

void TwinGLWidget::set_twin(TwinGLWidget *t) {
	twin = t;
}

void TwinGLWidget::set_display(float aX, float aY, float d) {
	angleX = aX;
	angleY = aY;
	distance = d;

	makeCurrent();
	set_modelview();
	doneCurrent();
	update();
}

void TwinGLWidget::set_n_iterations(size_t n) {
	nit = n;
}

void TwinGLWidget::set_lambda(float l) {
	lambda = l;
	mu = 1.0f/(0.1f - 1.0f/lambda);
}

void TwinGLWidget::set_smoothing_operator(const smoothing_operator& o) {
	op = o;
}

void TwinGLWidget::set_weight_type(const smooth_weight& w) {
	wt = w;
}

// OTHERS

void TwinGLWidget::run_smoothing_algorithm() {
	timing::time_point begin = timing::now();
	if (op == smoothing_operator::Laplacian) {
		algorithms::smoothing::laplacian(wt, lambda, nit, mesh);
	}
	else if (op == smoothing_operator::biLaplacian) {

	}
	else if (op == smoothing_operator::Taubin) {

	}
	timing::time_point end = timing::now();

	// output execution time
	cout << "Smoothed with operator ";
	if (op == smoothing_operator::Laplacian) {
		cout << "'Laplacian'";
	}
	else if (op == smoothing_operator::biLaplacian) {
		cout << "'biLaplacian'";
	}
	else if (op == smoothing_operator::Taubin) {
		cout << "'Taubin'";
	}
	cout << "curvature computed in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;

	// update mesh
	makeCurrent();
	mesh.free_buffers();
	mesh.init(program);
	doneCurrent();
	update();
}
