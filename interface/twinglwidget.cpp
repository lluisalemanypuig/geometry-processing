#include "twinglwidget.hpp"

// PUBLIC

TwinGLWidget::TwinGLWidget(QWidget *parent) : GLWidget(parent) {
	op = smoothing_operator::Laplacian;
}

TwinGLWidget::~TwinGLWidget() {

}

// SETTERS

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
	if (op == smoothing_operator::Laplacian) {
		algorithms::smoothing::laplacian(wt, lambda, nit, mesh);
	}
	else if (op == smoothing_operator::biLaplacian) {

	}
	else if (op == smoothing_operator::Taubin) {

	}

	makeCurrent();
	mesh.free_buffers();
	mesh.init(program);
	doneCurrent();
	update();
}
