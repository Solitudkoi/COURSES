#include "edge.h"
#include "vertex.h"

/**
 * @brief Registers this edge with all its dependent vertices by using
 *        the "Template Method" pattern, i.e., calling the virtual
 *        get_dependencies() method to get the list of vertices and then
 *        registering this edge with each vertex.
 */
void Edge::register_with_dependencies() {
  // [todo] Implement this method to register this edge with its dependent vertices.
}

/**
 * @brief Unregisters this edge from all its dependent vertices by using
 *        the "Template Method" pattern, i.e., calling the virtual
 *        get_dependencies() method to get the list of vertices and then
 *        unregistering this edge from each vertex.
 */
void Edge::unregister_from_dependencies() {
  // [todo] Implement this method to unregister this edge from its dependent vertices.
}