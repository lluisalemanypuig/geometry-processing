#pragma once

// C includes
#include <assert.h>

// C++ includes
#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// glm includes
#include <glm/glm.hpp>
using namespace glm;

class TriangleMesh {
	protected:
		/// The set of vertices of the mesh.
		vector<vec3> vertices;
		/**
		 * @brief The set of triangles of the mesh.
		 *
		 * Every three consecutive values starting at a
		 * position multiple of 3 contain the indexes of
		 * the vertices (stored in @ref vertices) of the
		 * triangle.
		 *
		 * Then, the @e i-th triangle has vertices
		 * @ref triangles[i], @ref triangles[i+1], @ref triangles[i+2]
		 */
		vector<int> triangles;
		/**
		 * @brief The set of opposite corners.
		 *
		 * This table stores for every corner its opposite
		 * corner. Therefore, we can access the vertex opposite
		 * to another vertex (with respect to an edge).
		 *
		 * Then @ref opposite_corners[@e i] = @e j iff corner @e j
		 * is the opposite corner of @e i.
		 */
		vector<int> opposite_corners;
		/**
		 * @brief Corner table.
		 *
		 * Relates each vertex of the mesh to a corner.
		 *
		 * A vertex may belong to several corners (as many
		 * as triangles it is found in), but this table
		 * associates each vertex to a single corner index.
		 */
		vector<int> corners;
		/**
		 * @brief Hard boundary of the mesh.
		 *
		 * The hard boundary is composed by pairs of vertices
		 * (each identified with a vertex index).
		 *
		 * The hard is boundary is defined as those edges ({vi,vj})
		 * of those triangles ({vi,vj,vk}) such that vk has no
		 * opposite corner.
		 */
		vector<pair<int,int> > boundary;

		/// Returns the area of the triangle made by the @e i-th , @e j-th and @e k-th vertices.
		float triangle_area(int i, int j, int v) const;

	public:
		/// Default constructor
		TriangleMesh();
		/// Destructor
		virtual ~TriangleMesh();

		// MODIFIERS

		/**
		 * @brief Adds a vertex to the mesh.
		 *
		 * A simple call to push_back of @ref vertices.
		 */
		void addVertex(const vec3& position);
		/**
		 * @brief Adds a triangle to the mesh.
		 *
		 * Three calls to push_back of @ref triangles.
		 */
		void addTriangle(int v0, int v1, int v2);

		/**
		 * @brief Builds the necessary tables to iterate through
		 * the one-ring of a vertex, ....
		 *
		 * Builds @ref corners and @ref opposite_corner tables.
		 * Also, just for the sake of completeness, it computes
		 * the boundary of the mesh in @ref boundary.
		 *
		 * This function should be called only after all
		 * vertices and triangles have been added.
		 */
		void make_neighbourhood_data();

		/**
		 * @brief Frees all the memoery occupied by the mesh.
		 *
		 * Clears the contents of @ref vertices, @ref triangles,
		 * @ref opposite_corners, @ref corners and @ref boundary.
		 */
		void destroy();

		// GETTERS

		/// Returns the number of vertices.
		size_t n_vertices() const;

		/// Returns the number of faces.
		size_t n_faces() const;

		/**
		 * @brief Computes the Mean curvature for each vertex.
		 *
		 * @e Kh is approximated using a discretisation of the
		 * Laplace-beltrami operator.
		 * @param[out] Kh @e Kh[i] contains an approximation of
		 * the Mean curvature at vertex i.
		 */
		void compute_Kh(vector<float>& Kh) const;

		/**
		 * @brief Computes the Gaussian curvature for each vertex.
		 *
		 * @e Kg is approximated using a discretisation of the
		 * Laplace-beltrami operator.
		 * @param[out] Kg @e Kg[i] contains an approximation of
		 * the Gaussian curvature at vertex i.
		 */
		void compute_Kg(vector<float>& Kg) const;
};

