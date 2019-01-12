
/**
 * @brief Main namespace of the library.
 * 
 * In this namespace is defined the main data structure used to
 * implement all the algorithms: the @ref TriangleMesh.
 * 
 * In order to read a mesh from disk, see namespace @ref PLY_reader.
 * 
 * All the algorithms are defined in their corresponding namespaces.
 */
namespace geoproc {

	/**
	 * @brief Classes used as iterators over features of
	 * a triangular mesh.
	 */
	namespace iterators {

	/// Iterate over vertices.
	namespace vertex { }

	} // -- namespace iterators

	/**
	 * @brief Implementation of different curvature algorithms.
	 * 
	 * The following curvatures are available:
	 * - @ref curvature::Gauss(const TriangleMesh&, std::vector<double>&, double*, double*).
	 * - @ref curvature::Gauss(const TriangleMesh&, std::vector<double>&, size_t).
	 * - @ref curvature::Gauss(const TriangleMesh&, std::vector<double>&, size_t, double*, double*).
	 * - @ref curvature::mean(const TriangleMesh&, std::vector<double>&, double*, double*).
	 * - @ref curvature::mean(const TriangleMesh&, std::vector<double>&, size_t).
	 * - @ref curvature::mean(const TriangleMesh&, std::vector<double>&, size_t, double*, double*).
	 */
	namespace curvature { }

	/**
	 * @brief Implementation of different smoothing algorithms.
	 * 
	 * Iterative, local algorithms, and global algorithms.
	 */
	namespace smoothing {

	/**
	 * @brief Local smoothing algorithms.
	 * 
	 * These can also be referred to as iterative smoothing algorithms.
	 */
	namespace local {}

	/**
	 * @brief Local smoothing algorithms core functions.
	 * 
	 * Here are defined the core functions that the algorithms in namespace
	 * @ref local (and others too) use.
	 */
	namespace local_private {}

	/**
	 * @brief Global smoothing algorithms.
	 * 
	 * 
	 */
	namespace global {}

	} // -- namespace smoothing

	/**
	 * @brief Frequencies filtering.
	 * 
	 * Implementation of algorithms to highlight certain frequencies
	 * of a mesh.
	 */
	namespace filter_frequencies { }

	/**
	 * @brief Parametrisation algorithms.
	 * 
	 * Implemented:
	 * - Harmonic maps (see @ref harmonic_maps).
	 */
	namespace parametrisation { }

	/**
	 * @brief Remeshing algorithms
	 * 
	 * Implemented:
	 * - Using Harmonic maps' parametrisation (see @ref harmonic_maps).
	 */
	namespace remeshing { }
    
}
