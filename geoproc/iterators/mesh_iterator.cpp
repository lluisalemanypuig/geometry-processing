#include <geoproc/iterators/mesh_iterator.hpp>

namespace geoproc {
namespace iterators {

mesh_iterator::mesh_iterator(const TriangleMesh& m) : mesh(m) {

}

mesh_iterator::~mesh_iterator() { }

} // -- namespace iterators
} // -- namespace geoproc
