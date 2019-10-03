/*********************************************************************
 * Geometry Processing Project
 * Copyright (C) 2018-2019 Lluís Alemany Puig
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: Lluis Alemany Puig (lluis.alemany.puig@gmail.com)
 *
 ********************************************************************/

#pragma once

// Qt includes
#include <QJsonDocument>
#include <QMouseEvent>

// geoproc includes
#include <geoproc/definitions.hpp>
using namespace geoproc;

// Custom includes
#include "render_triangle_mesh.hpp"
#include "glwidget.hpp"
#include "utils.hpp"

class TwinGLWidget : public GLWidget {
	protected:
		modifier op;
		weight wt;
		size_t nit;
		double lambda;
		double mu;

		double perc_fix_vertices;

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
		void set_lambda(double l);
		void set_perc_fixed_vertices(double p);
		void set_smooth_operator(const modifier& o);
		void set_smooth_weight_type(const weight& w);

		// OTHERS

		void make_remeshing(size_t N, size_t M);
		void run_local_smoothing_algorithm();
		void run_global_smoothing_algorithm();
		void run_band_frequencies(const QJsonDocument& doc);
};
