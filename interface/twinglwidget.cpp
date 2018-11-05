#include "twinglwidget.hpp"

// C++ includes
#include <iostream>
using namespace std;

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
	mesh.make_neighbourhood_data();
	mesh.make_angles_area();

	cout << "Smoothed with operator ";
	timing::time_point begin = timing::now();
	if (op == smoothing_operator::Laplacian) {
		cout << "'Laplacian'";
		algorithms::smoothing::laplacian(wt, lambda, nit, nt, mesh);
	}
	else if (op == smoothing_operator::biLaplacian) {
		cout << "'biLaplacian'";
		algorithms::smoothing::bilaplacian(wt, lambda, nit, nt, mesh);
	}
	else if (op == smoothing_operator::Taubin) {
		cout << "'Taubin'";
		algorithms::smoothing::TaubinLM(wt, lambda, nit, nt, mesh);
	}
	timing::time_point end = timing::now();

	// output execution time
	cout << " in "
		 << timing::elapsed_seconds(begin,end)
		 << " seconds" << endl;

	if (current_curv_display != curv_type::none) {
		compute_curvature();
	}

	// update mesh
	if (current_curv_display != curv_type::none) {

		// show curvature already has
		// makeCurrent() .. doneCurrent()
		show_curvature(false, true);
	}
	else {
		makeCurrent();
		mesh.make_vertices(program);
		doneCurrent();
	}
	update();
}
