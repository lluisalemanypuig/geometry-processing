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

// C++ includes
#include <vector>

// Qt includes
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QMouseEvent>

// geoproc includes
#include <geoproc/definitions.hpp>

// Custom includes
#include "render_triangle_mesh.hpp"

enum class polymode : int8_t {
	none = -1,
	// fill the triangles of the meshes
	solid,

	// the meshes in wireframe mode
	wireframe,

	// render reflection lines on the mesh
	reflection_lines,

	/* Harmonic Maps */

	// Render a regular grid using texture
	// coordinates generated generated by a
	// harmonic map.
	harmonic_maps,

	// Render a regular grid using texture
	// coordinates generated generated by a
	// "cicular" harmonic map
	harmonic_maps_Circle,

	// Render a regular grid using texture
	// coordinates generated generated by a
	// "square" harmonic map
	harmonic_maps_Square
};

enum class curv_type : int8_t {
	none = -1,
	Gauss,	// display Kg = k1*k2
	Mean	// display Kh = (k1 + k2)/2
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
	protected:
		/// Name of the widget. For debugging.
		string name;

		/// Display the mesh in a certain polygon mode. See @ref polymode.
		polymode to_polymode;
		polymode current_polymode;
		polymode harmonic_maps_mode;
		weight harmonic_maps_weight;
		/// Type of curvature to be displayed. See @ref curvature_display.
		curv_type current_curv_display;
		curv_type to_curv_display;

		float angleX, angleY, distance;
		QPoint lastMousePos;

		QOpenGLShaderProgram *program;
		RenderTriangleMesh mesh;

		/// Minimum and maximum curvature values
		double cm, cM;
		/// Values of the curvature per vertex
		std::vector<double> curvature_values;
		/// Proportion of curvature values to be used.
		double prop;
		double to_prop;

		/// Number of threads to use.
		size_t nt;

	protected:
		void set_projection();
		void set_modelview();

		void delete_program();
		void load_shader();

		void compute_curvature();
		void show_curvature(bool make_all_buffers = true);

		void compute_harmonic_maps();

		/**
		 * @brief Initialises a mesh.
		 * @param make_neigh True if neighbourhood data is to be made.
		 */
		void init_mesh(bool make_all_buffers);

		void initializeGL();
		void resizeGL(int w, int h);
		void paintGL();

		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);

	public:
		GLWidget(QWidget *parent);
		virtual ~GLWidget();

		/**
		 * @brief Copies mesh @e rm.
		 *
		 * Frees the memory occupied by the current mesh.
		 * The mesh passed as parameter is not scaled to unit.
		 * @param rm
		 */
		void set_mesh(const RenderTriangleMesh& rm, bool init = true);
		/// Loads a mesh from a file.
		void load_mesh(const QString& filename);
		/// Clears the mesh.
		void clear_mesh();

		void set_light_sources_refl_lines(int v);
		void set_name(const string& s);

		/// Sets the polygon display mode. See @ref pm.
		void set_polygon_mode(const polymode& current_polymode);
		/// Changes the polygon mode to the current.
		void change_polygon_mode();
		/// Set the type of harmonic map generated.
		void set_harmonic_map
		(const polymode& current_polymode, const weight& weight);
		/**
		 * @brief Render a wireframe for harmonic maps.
		 * @pre The widget must be rendering a harmonic type.
		 */
		void set_harmonic_map_remeshing(bool r);
		/// Sets the type of curvature to be displayed. See @ref curvature_display.
		void set_curvature_display(const curv_type& current_curv_display);
		/**
		 * @brief Displays the currently selected curvature.
		 *
		 * Loads the appropriate shader programs. Calculates curvature
		 * if necessary.
		 */
		void change_curvature_display();

		/// Sets the proportion (in %) of curvature values to be used for display.
		/// Displays the curvature values afterwards.
		void change_display_curvature_proportion(double p);
		/// Sets the proportion (in %) of curvature values to be used for display.
		void change_curvature_proportion(double p);

		/// Sets the number of threads.
		void set_num_threads(size_t nt);

		/// Returns a constant reference to the mesh in this widget.
		const RenderTriangleMesh& get_mesh() const;

		const polymode& get_polygon_mode() const;
		const curv_type& get_curvature_display() const;
};

