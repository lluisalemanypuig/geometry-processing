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

	public:
		/// Default constructor
		TriangleMesh();
		/// Destructor
		virtual ~TriangleMesh();

		// MODIFIERS

		/**
		 * @brief Adds a collection of vertices to the mesh.
		 *
		 * Each vertex starts at a position multiple of 3.
		 * Then, the vertices are added to @ref vertices.
		 * @param coords The coordinates of all the vertices.
		 * The size must be a multiple of 3.
		 */
		void set_vertices(const vector<float>& coords);
		/**
		 * @brief Adds a collection of vertices to the mesh.
		 *
		 * The vertices passed in @e vs are added to @ref vertices.
		 */
		void set_vertices(const vector<vec3>& vs);
		/**
		 * @brief Adds the connectivity information to the mesh.
		 *
		 * The contents of @e tris are the vertex indices of the
		 * triangles of the mesh.
		 *
		 * That is, face @e f has vertices @ref vertices[3*@e f],
		 * @ref vertices[3*@e f + 1], @ref vertices[3*@e f + 2].
		 */
		void set_triangles(const vector<int>& tris);

		/**
		 * @brief Reescales the mesh so that it fits in a unit box.
		 *
		 * The mesh is reescaled so that the longest length of the
		 * sides of the bounding box equals 1.
		 */
		void scale_to_unit();

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

		/// Returns the number of triangles.
		size_t n_triangles() const;

		/**
		 * @brief Returns the index of the vertex corresponding to corner @e c.
		 * @param c Corner index: 0 <= @e c < number of triangles.
		 */
		int get_vertex_corner(int c) const;

		/**
		 * @brief Returns a corner index for vertex @e v.
		 * @param v A valid vertex index: 0 <= @e v < number of vertices.
		 * @pre Neighbourhood data must exist in this mesh.
		 * See @ref make_neighbourhood_data.
		 * @return Returns a valid corner index (unless there are
		 * no faces in this mesh). Returns @e c such that
		 * 0 <= @e c < number of triangles
		 */
		int get_corner_vertex(int v) const;

		/**
		 * @brief Returns a face index for corner @e c.
		 * @param c A valid corner index: 0 <= @e c < number of triangles.
		 * @pre Neighbourhood data must exist in this mesh.
		 * See @ref make_neighbourhood_data.
		 * @return If the corner index is valid, the face index @e f
		 * returned is also valid: 0 <= @e 3*f < number of triangles.
		 */
		int get_face_corner(int c) const;

		/**
		 * @brief Returns the indexes of the vertices of a face.
		 *
		 * It is guaranteed that the order of the vertices is
		 * the same as it is given in the loaded file.
		 * @param f A valid face index: 0 <= @e 3*f < number of triangles.
		 * @param[out] v1 The first vertex index of the face.
		 * @param[out] v2 The second vertex index of the face.
		 * @param[out] v3 The third vertex index of the face.
		 */
		void get_vertices_face(int f, int& v0, int& v1, int& v2) const;

		/**
		 * @brief Returns the sorted indexes of the vertices of a face.
		 *
		 * It is guaranteed that the order of the vertices is
		 * the same as it is given in the loaded file.
		 *
		 * The vertices are sorted so that the first is equal to vertex
		 * index @e v
		 * @param f A valid face index: 0 <= @e 3*f < number of triangles.
		 * @param[out] v1 The first vertex index of the face.
		 * @param[out] v2 The second vertex index of the face.
		 * @param[out] v3 The third vertex index of the face.
		 */
		void get_vertices_face(int f, int v, int& v1, int& v2, int& v3) const;

		/**
		 * @brief Returns the corner opposite to corner c.
		 * @param c Corner index: 0 <= @e c < number of triangles.
		 * @pre Neighbourhood data must exist in this mesh.
		 * See @ref make_neighbourhood-data.
		 * @return Returns -1 if there is no opposite corner (the
		 * case of hard boundary). Returns a valid corner index
		 * otherwise.
		 */
		int get_opposite_corner(int c) const;

		/**
		 * @brief Returns the coordinates of a vertex.
		 * @param v A valid vertex index: 0 <= @e v < number of vertices.
		 */
		const vec3& get_vertex(int v) const;

		/**
		 * @brief Returns the area of face @e f.
		 *
		 * First, retrieves the vertices of the face with
		 * @ref get_vertices_face and then calls
		 * @ref get_triangle_area(int,int,int)const.
		 * @param f A valid face index.
		 */
		float get_triangle_area(int f) const;

		/**
		 * @brief Assuming the vertices make a triangle, return its area.
		 * @param i A valid vertex index.
		 * @param j A valid vertex index.
		 * @param k A valid vertex index.
		 */
		float get_triangle_area(int i, int j, int k) const;

};

