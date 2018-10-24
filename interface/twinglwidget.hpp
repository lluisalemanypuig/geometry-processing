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
#include <algorithms/smoothing/smoothing.hpp>
using namespace algorithms;
using namespace smoothing;

// Custom includes
#include "render_triangle_mesh.hpp"
#include "glwidget.hpp"
#include "utils.hpp"

enum class smoothing_operator : int8_t {
	none = -1,
	Laplacian,		// Laplacian smoothing algorithm
	biLaplacian,	// Bi-Laplacian smoothing algorithm
	Taubin			// Taubin l-m operator
};

class TwinGLWidget : public GLWidget {
	private:
		smoothing_operator op;
		smooth_weight wt;
		size_t nit;
		float lambda;
		float mu;

	public:
		TwinGLWidget(QWidget *parent);
		~TwinGLWidget();

		// SETTERS

		void set_n_iterations(size_t n);
		void set_lambda(float l);
		void set_smoothing_operator(const smoothing_operator& o);
		void set_weight_type(const smooth_weight& w);

		// OTHERS

		void run_smoothing_algorithm();
};
