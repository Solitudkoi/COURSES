#ifndef EDGE_H
#define EDGE_H

#include <vector>
#include "entity.h"

class Vertex; // Forward declaration

/**
 * @brief Abstract base class for all edge entities (lines, circles, arcs)
 *        in the sketch. Edges depend on one or more vertices, which they
 *        reference. This class provides an interface for retrieving these
 *        dependencies. Registration/Unregistration with dependent vertices
 *        is handled through the helper methods.
 */
class Edge : public Entity {
public:
  using Entity::Entity; // Inherit constructor

  // Get the list of vertices this edge depends on
  virtual std::vector<Vertex *> get_dependencies() const = 0;

  // Methods for derived classes to register/unregister with dependencies
  void register_with_dependencies();
  void unregister_from_dependencies();
};

#endif // EDGE_H