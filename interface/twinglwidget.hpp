#pragma once

// Qt includes
#include <QJsonDocument>
#include <QMouseEvent>

// geoproc includes
#include <geoproc/smoothing/smoothing_defs.hpp>
using namespace geoproc;

// Custom includes
#include "render_triangle_mesh.hpp"
#include "glwidget.hpp"
#include "utils.hpp"

class TwinGLWidget : public GLWidget {
	protected:
		smoothing::smooth_operator op;
		smoothing::smooth_weight wt;
		size_t nit;
		float lambda;
		float mu;

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

		TwinGLWidget *twin;

	public:
		TwinGLWidget(QWidget *parent);
		~TwinGLWidget();

		// SETTERS

		void set_twin(TwinGLWidget *t);
		void set_display(float aX, float aY, float d);

		void set_n_iterations(size_t n);
		void set_lambda(float l);
		void set_smooth_operator(const smoothing::smooth_operator& o);
		void set_smooth_weight_type(const smoothing::smooth_weight& w);

		// OTHERS

		void run_smoothing_algorithm();
		void run_high_freqs_algorithm();
		void run_exagg_high_freqs_algorithm();

		void run_band_frequencies(const QJsonDocument& doc);
};
