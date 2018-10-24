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
#include <algorithms/curvature/curvature.hpp>
#include <algorithms/ply_reader.hpp>

// Custom includes
#include "render_triangle_mesh.hpp"
#include "utils.hpp"

enum class polymode : int8_t {
	none = -1,
	solid,				// fill the triangles of the meshes
	wireframe,			// the meshes in wireframe mode
	reflection_lines	// render reflection lines on the mesh
};

enum class curv_type : int8_t {
	none = -1,
	Gauss,	// display Kg = k1*k2
	Mean	// display Kh = (k1 + k2)/2
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
	protected:
		/// Display the mesh in a certain polygon mode. See @ref polymode.
		polymode pm;
		/// Type of curvature to be displayed. See @ref curvature_display.
		curv_type curv_display;

		float angleX, angleY, distance;
		QPoint lastMousePos;

		QOpenGLShaderProgram *program;
		RenderTriangleMesh mesh;

		/// Values of the curvature per vertex
		vector<float> curvature_values;

		/// Number of threads to use.
		size_t nt;

	protected:
		void set_projection();
		void set_modelview();

		void delete_program();
		void load_simple_shader();
		void load_curvature_shader();

		void compute_curvature();
		void show_curvature(bool load_shader = false);

		/**
		 * @brief Initialises a mesh.
		 * @param make_neigh True if neighbourhood data is to be made.
		 */
		void init_mesh(bool make_neigh);

		void initializeGL();
		void resizeGL(int w, int h);
		void paintGL();

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

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
		void set_mesh(const RenderTriangleMesh& rm);
		/// Loads a mesh from a file.
		void load_mesh(const QString& filename);
		/// Clears the mesh.
		void clear_mesh();

		/// Sets the polygon display mode. See @ref pm.
		void set_polygon_mode(const polymode& pm);
		/// Sets the type of curvature to be displayed. See @ref curvature_display.
		void set_curvature_display(const curv_type& curv_display);

		/// Sets the number of threads.
		void set_num_threads(size_t nt);

		/// Returns a constant reference to the mesh in this widget.
		const RenderTriangleMesh& get_mesh() const;
};

