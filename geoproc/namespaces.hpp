
/**
 * @brief Classes used as iterators over features of
 * a triangular mesh.
 */
namespace iterators {
	
	/// Iterate over vertices.
	namespace vertex { }
	
	/**
	 * @brief Implementation of different curvature algorithms.
	 * 
	 * The following curvatures are available:
	 * - @ref curvature::Gauss
	 * - @ref curvature::mean
	 */
	namespace curvature { }
	
	/**
	 * @brief Implementation of different smoothing algorithms.
	 * 
	 * Iterative, local algorithms, and global algorithms.
	 */
	namespace smoothing { }
	
} // -- namespace iterators
