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

		float perc_fix_vertices;

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
		void set_perc_fixed_vertices(float p);
		void set_smooth_operator(const smoothing::smooth_operator& o);
		void set_smooth_weight_type(const smoothing::smooth_weight& w);

		// OTHERS

		void run_local_smoothing_algorithm();
		void run_global_smoothing_algorithm();
		void run_band_frequencies(const QJsonDocument& doc);
};
