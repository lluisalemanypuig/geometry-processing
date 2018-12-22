#pragma once

// C++ includes
#include <vector>

// glm includes
#include <glm/glm.hpp>

// geoproc includes
#include <geoproc/mesh_edge.hpp>

namespace geoproc {

/**
 * @brief Implementation of a triangular mesh.
 *
 * It contains the vertices of a triangular mesh (@ref vertices) which
 * are related using indices (@ref triangles), hence following the usual
 * implementation.
 *
 * It also stores normal vectors per triangle (@ref normal_vectors), angles
 * and area of each triangle (@ref angles, @ref areas), and implements the
 * corner table data structure (see @ref corners, @ref opposite_corners).
 *
 * Finally, it holds other interesting information about the structure of the
 * mesh, like boundaries (see @ref boundary_edges, @ref boundaries).
 */
class TriangleMesh {
	protected:
		/// The set of vertices of the mesh.
		std::vector<glm::vec3> vertices;
		/**
		 * @brief The set of triangles of the mesh.
		 *
		 * Every three consecutive values starting at a
		 * position multiple of 3 contain the indexes of
		 * the vertices (stored in @ref vertices) of the
		 * triangle.
		 *
		 * Then, the @e i-th triangle has vertices
		 * @ref triangles[i], @ref triangles[i+1], @ref triangles[i+2].
		 *
		 * Just to make it clear: @ref triangles contains indices
		 * pointing to values in @ref vertices.
		 */
		std::vector<int> triangles;

		/**
		 * @brief Normal vector for every triangle of the mesh.
		 *
		 * The @e i-th vector of this container corresponds to the normal
		 * vector of the @e i-th triangle of the mesh. This container
		 * can be filled after having set the vertices and the
		 * triangles.
		 */
		std::vector<glm::vec3> normal_vectors;

		/**
		 * @brief Angles of each triangle.
		 *
		 * Position @e i contains three values:
		 * - angle <1,0,2> (angle incident to 0)
		 * - angle <0,1,2> (angle incident to 1)
		 * - angle <1,2,0> (angle incident to 2)
		 *
		 * where 0,1,2 represent, respectively, the
		 * first, second and third vertex of the
		 * @e i-th triangle.
		 *
		 * Notice that its size is @ref n_triangles() or
		 * @ref n_corners()/3.
		 */
		std::vector<glm::vec3> angles;

		/**
		 * @brief Area of each triangle.
		 *
		 * Notice that its size is @ref n_triangles() or
		 * @ref n_corners()/3.
		 */
		std::vector<float> areas;

		/**
		 * @brief The set of opposite corners.
		 *
		 * This table stores for every corner its opposite
		 * corner. Therefore, we can access the vertex opposite
		 * to another vertex (with respect to an edge).
		 *
		 * In general, @ref opposite_corners[@e i] = @e j iff
		 * corner @e j is the opposite corner of @e i.
		 *
		 * In some cases this is not true, since the "opposite"
		 * corner is found by crossing a boundary. Such opposite
		 * is given as a negative value, but less than the number of
		 * triangles in absolute value if @ref make_neighbourhood_data
		 * was called with a 'true' value. In this case, we need to
		 * consider orientations: take its opposite edge and the 'right'
		 * shared vertex (when placing the corner 'on top of' the
		 * opposite edge), now take the other boundary edge incident
		 * to the 'right' edge and, finally, this edge's opposite corner.
		 * The corner found is defined to be opposite of the starting
		 * corner. Notice that this is defined for counterclockwise
		 * traversals on the one-ring neighbourhood of the 'right' vertex.
		 */
		std::vector<int> opposite_corners;

		/**
		 * @brief Corner table.
		 *
		 * Relates each vertex of the mesh to a corner.
		 *
		 * A vertex may belong to several corners (as many
		 * as triangles it is found in), but this table
		 * associates each vertex to a single corner index.
		 */
		std::vector<int> corners;
		/**
		 * @brief Edges of this mesh.
		 *
		 * This vector is filled in the @ref make_neighbourhood_data
		 * function. The contents of this container are valid only when
		 * @ref is_neighbourhood_valid evaluates to true.
		 *
		 * The members of the class @ref mesh_edge make this container
		 * a DCEL.
		 */
		std::vector<mesh_edge> all_edges;

		/**
		 * @brief Edge index for each vertex.
		 *
		 * Each vertex is incident to several edges. This container
		 * relates each vertex to a asingle edge. Therefore, this container
		 * has as many edge indices as vertices are in this mesh.
		 *
		 * These indices point to values in @ref all_edges.
		 */
		std::vector<int> vertex_edge;

		/**
		 * @brief Boundary edges of the mesh.
		 *
		 * The hard boundary is composed by pairs of vertices
		 * (each identified with a vertex index).
		 *
		 * The hard is boundary is defined as those edges ({@e vi,@e vj})
		 * of those triangles ({@e vi,@e vj,@e vk}) such that @e vk has no
		 * opposite corner. All @e vi, @e vj, @e vk are valid vertex
		 * indices.
		 *
		 * Boundary vertices are collected into this container when the
		 * neighbourhood data is made (see @ref make_neighbourhood_data).
		 * In order to make all boundary lists call @ref make_boundaries,
		 * which will fill the member @ref boundaries.
		 */
		std::vector<int> boundary_edges;

		/**
		 * @brief Boundaries of this mesh.
		 *
		 * Call method @ref make_boundaries to have this container filled
		 * with the appropriate data.
		 *
		 * This container contains as many lists of vertex indices
		 * as boundaries are in the mesh.
		 */
		std::vector<std::vector<int> > boundaries;

		/// Minimum value of x-,y-, and z-coordinates.
		glm::vec3 min_coord;
		/// Maximum value of x-,y-, and z-coordinates.
		glm::vec3 max_coord;

		/// Are the computed triangle angles and areas valid?
		bool angles_area_valid;
		/**
		 * @brief Is the neighbourhood data valid?
		 *
		 * Is the data in @ref corners, @ref opposite_corners
		 * and @ref boundary_edges valid?
		 */
		bool neigh_valid;
		/**
		 * @brief Are the boundaries of the mesh valid?
		 *
		 * Is the data in @ref boundaries valid?
		 */
		bool boundaries_valid;

	protected:
		/// Sets to false the attribute @ref angles_area_valid.
		void invalidate_areas_angles();
		/// Sets to false the attribute @ref neigh_valid.
		void invalidate_neighbourhood();
		/// Sets to false the attribute @ref boundaries_valid.
		void invalidate_boundaries();

		/**
		 * @brief Invalidates the whole state of the mesh.
		 *
		 * Sets to false all validty attributes:
		 * - @ref angles_area_valid
		 * - @ref neigh_valid
		 */
		void invalidate_state();

		/**
		 * @brief Copies mesh @e m contents into this one.
		 *
		 * This mesh is previously freed.
		 * @param m Mesh to be copied.
		 */
		void copy_mesh(const TriangleMesh& m);

		/**
		 * @brief Assuming the vertices make a triangle, return its area.
		 * @param i A valid vertex index.
		 * @param j A valid vertex index.
		 * @param k A valid vertex index.
		 */
		float get_triangle_area(int i, int j, int k) const;

	public:
		/// Default constructor.
		TriangleMesh();
		/// Copy constructor.
		TriangleMesh(const TriangleMesh& m);
		/// Destructor
		virtual ~TriangleMesh();

		// MODIFIERS

		/**
		 * @brief Sets the vertices to the mesh.
		 *
		 * Each vertex starts at a position multiple of 3.
		 * Then, the contents of @ref vertices is set to the
		 * contents of @e vs.
		 *
		 * It also, computes the minimum and maximum coordinates
		 * (see @ref min_coord, and @ref max_coord).
		 * @param coords The coordinates of all the vertices.
		 * The size must be a multiple of 3.
		 *
		 * @post The angles and areas of the triangles are invalidated.
		 */
		void set_vertices(const std::vector<float>& coords);
		/**
		 * @brief Sets the vertices to the mesh.
		 *
		 * The contents of @ref vertices is set to the contents of
		 * @e vs. It also, computes the minimum and maximum coordinates
		 * (see @ref min_coord, and @ref max_coord).
		 * @pre @e vs is not null and points to the first element
		 * of an array of @e N vertices.
		 *
		 * @post The angles and areas of the triangles are invalidated.
		 */
		void set_vertices(const glm::vec3 *vs, int N);
		/**
		 * @brief Sets the vertices to the mesh.
		 *
		 * The contents of @ref vertices is set to the contents of
		 * @e vs.
		 *
		 * @post The angles and areas of the triangles are invalidated.
		 */
		void set_vertices(const std::vector<glm::vec3>& vs);
		/**
		 * @brief Adds the connectivity information to the mesh.
		 *
		 * The contents of @e tris are the vertex indices of the
		 * triangles of the mesh. That is, face @e f has vertices
		 * @ref vertices[3*@e f], @ref vertices[3*@e f + 1],
		 * @ref vertices[3*@e f + 2].
		 *
		 * Once the triangles have been copied, computes the
		 * angles of each triangle. See @ref angles for details.
		 * Also, computes the areas of each of the triangles
		 * (see @ref areas).
		 *
		 * @pre The vertices must have been set.
		 * @post The state of the mesh is invalidated (angles, area,
		 * neighbourhood data is not valid).
		 */
		void set_triangles(const std::vector<int>& tris);

		/**
		 * @brief Computes the normal of every triangle.
		 *
		 * Once the triangles and vertices have been set to the mesh,
		 * this method can compute the normal vectors for every triangle.
		 *
		 * The container is resized to be able to allocate as many
		 * normal vectors as triangles in the mesh.
		 *
		 * The normal is calculated as the normalisation of the cross
		 * product of the vectors from the first vertex of the triangle
		 * to the other two vertices:
		 *
		 * \f$ n_i = \frac{ u_1 \times u_2 }{ || u_1 \times u_2 || } \f$
		 *
		 * where \f$ u_1 = v_1 - v_0 \f$ and \f$ u_2 = v_2 - v_0 \f$,
		 * and \f$v_0,v_1,v_2\f$ are the first, second and third vertices
		 * of the triangle.
		 */
		void make_normal_vectors();

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
		 * Builds @ref corners and @ref opposite_corners tables.
		 * It also collects all boundary edges and stores them
		 * in the same "boundary list" in @ref boundaries. If one
		 * wants all boundaries as edge lists, call function
		 * @ref make_boundaries after this one.
		 *
		 * This function should be called only after all
		 * vertices and triangles have been added.
		 *
		 * If this function is called when the corresponding state
		 * is valid then this function does nothing.
		 */
		void make_neighbourhood_data();

		/**
		 * @brief Computes the angles and the areas of the triangles.
		 *
		 * Fills the containers @ref angles, @ref areas.
		 */
		void make_angles_area();

		/**
		 * @brief Computes the boundaries of this mesh.
		 *
		 * Fills container @ref boundaries.
		 * @pre Neighbourhood data must have been made (see
		 * @ref make_neighbourhood_data).
		 */
		void make_boundaries();

		/**
		 * @brief Frees all the memoery occupied by the mesh.
		 *
		 * Clears the contents of:
		 * - @ref vertices,
		 * - @ref triangles,
		 * - @ref normal_vectors,
		 * - @ref angles,
		 * - @ref areas,
		 * - @ref opposite_corners,
		 * - @ref corners,
		 * - @ref boundary_edges,
		 * - @ref boundaries,
		 * - @ref all_edges,
		 * - @ref vertex_edge
		 */
		void destroy();

		// GETTERS

		/// Returns the number of vertices.
		int n_vertices() const;
		/// Returns the number of edges.
		int n_edges() const;
		/// Returns the number of triangles.
		int n_triangles() const;
		/// Returns the number of corners.
		int n_corners() const;
		/// Returns the number of boundary edges.
		int n_boundary_edges() const;
		/// Returns the number of boundaries in this mesh.
		size_t n_boundaries() const;

		/**
		 * @brief Returns the index of the vertex corresponding to corner @e c.
		 * @param c Valid corner index: 0 <= @e c < number of corners.
		 */
		int get_vertex_corner(int c) const;

		/**
		 * @brief Returns a corner index for vertex @e v.
		 * @param v A valid vertex index: 0 <= @e v < number of vertices.
		 * @pre Neighbourhood data must be valid, i.e, the result of
		 * @ref is_neighbourhood_valid() must be true. If false,
		 * call @ref make_neighbourhood_data.
		 * @return Returns a valid corner index (unless there are
		 * no faces in this mesh). Returns @e c such that
		 * 0 <= @e c < number of triangles
		 */
		int get_corner_vertex(int v) const;

		/**
		 * @brief Returns the index of an edge incident to vertex @e v.
		 * @param v A valid vertex index: 0 <= @e v < number of vertices.
		 * @pre Neighbourhood data must be valid, i.e, the result of
		 * @ref is_neighbourhood_valid() must be true. If false,
		 * call @ref make_neighbourhood_data.
		 * @return Returns a valid edge index.
		 */
		int get_edge_vertex(int v) const;

		/**
		 * @brief Returns a triangle index for corner @e c.
		 * @param c A valid corner index: 0 <= @e c < number of corners.
		 * @pre Neighbourhood data must exist in this mesh.
		 * See @ref make_neighbourhood_data.
		 * @return If the corner index is valid, the face index @e f
		 * returned is also valid: 0 <= @e 3*f < number of triangles.
		 */
		int get_triangle_corner(int c) const;

		/**
		 * @brief Returns the indexes of the vertices of a triangle.
		 *
		 * It is guaranteed that the order of the vertices is
		 * the same as it is given in the loaded file.
		 * @param t A valid triangle index: 0 <= @e t < number of triangles.
		 * @param[out] v0 The first vertex index of the face.
		 * @param[out] v1 The second vertex index of the face.
		 * @param[out] v2 The third vertex index of the face.
		 */
		void get_corners_triangle(int t, int& v0, int& v1, int& v2) const;

		/**
		 * @brief Returns the indexes of the vertices of a triangle.
		 *
		 * It is guaranteed that the order of the vertices is
		 * the same as it is given in the loaded file.
		 * @param t A valid triangle index: 0 <= @e t < number of triangles.
		 * @param[out] v0 The first vertex index of the face.
		 * @param[out] v1 The second vertex index of the face.
		 * @param[out] v2 The third vertex index of the face.
		 */
		void get_vertices_triangle(int t, int& v0, int& v1, int& v2) const;

		/**
		 * @brief Returns the sorted indexes of the vertices of a face.
		 *
		 * The vertices are sorted so that the first is equal to vertex
		 * index @e v.
		 * @param t A valid triangle index: 0 <= @e t < number of triangles.
		 * @param v A vertex of the triangle @e t.
		 * @param[out] v0 The first vertex index of the face.
		 * @param[out] v1 The second vertex index of the face.
		 * @param[out] v2 The third vertex index of the face.
		 */
		void get_vertices_triangle(int t, int v, int& v0, int& v1, int& v2) const;

		/**
		 * @brief Returns the corner opposite to corner c.
		 * @param c Valid corner index: 0 <= @e c < number of corners.
		 * @pre Neighbourhood data must be valid, i.e, the result of
		 * @ref is_neighbourhood_valid() must be true. If false,
		 * call @ref make_neighbourhood_data.
		 * @return Returns -1 if there is no opposite corner (the
		 * case of hard boundary). Returns a valid corner index
		 * otherwise.
		 */
		int get_opposite_corner(int c) const;

		/**
		 * @brief Returns the coordinates of a vertex.
		 * @param v A valid vertex index: 0 <= @e v < number of vertices.
		 */
		const glm::vec3& get_vertex(int v) const;

		/// Returns a constant reference to all vertices of the mesh.
		const std::vector<glm::vec3>& get_vertices() const;
		/// Returns a constant reference to all vertices of the mesh.
		const glm::vec3 *get_pvertices() const;

		/**
		 * @brief Returns the area of face @e f.
		 *
		 * First, retrieves the vertices of the face with
		 * @ref get_vertices_triangle and then calls
		 * @ref get_triangle_area(int,int,int)const.
		 * @param t A valid triangle index: 0 <= @e t < number of triangles.
		 */
		float get_triangle_area(int t) const;

		/// Returns the area of each triangle.
		const std::vector<float>& get_areas() const;

		/**
		 * @brief Returns the angles on each triangle.
		 * @pre Angles and area data must be valid
		 */
		const std::vector<glm::vec3>& get_angles() const;

		/// Is neighbourhood data valid?
		bool are_angles_area_valid() const;
		/// Is neighbourhood data valid?
		bool is_neighbourhood_valid() const;
		/// Are the boundaries valid?
		bool are_boundaries_valid() const;

		/// Returns a relation between vertices and edges.
		const std::vector<int>& get_vertex_edge() const;
		/// Returns all edges in the mesh.
		const std::vector<mesh_edge>& get_edges() const;
		/// Returns the boundary edges in this mesh.
		const std::vector<int>& get_boundary_edges() const;
		/// Returns the boundaries in this mesh.
		const std::vector<std::vector<int> >& get_boundaries() const;

		/**
		 * @brief Returns the minimum and maximum coordinates.
		 * @param[out] m Minimum x-,y-,z- coordinates.
		 * @param[out] M Maximum x-,y-,z- coordinates.
		 */
		void get_min_max_coordinates(glm::vec3& m, glm::vec3& M) const;

};

} // -- namespace geoproc
