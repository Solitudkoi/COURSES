#ifndef VERTEX_H
#define VERTEX_H

#include <set>
#include <string>
#include "entity.h"
#include "point.h"

class Edge; // Forward declaration

/**
 * @brief Represents a vertex in 2D space, containing its coordinates and
 *        a list of dependent edges, which are edges that reference this vertex.
 */
class Vertex : public Entity {
private:
  double x_;
  double y_;
  std::set<Edge *> dependents_; // Edges that depend on this vertex

public:
  Vertex(EntityId id, double x, double y)
      : Entity(id), x_(x), y_(y) {}

  // Accessors
  double x() const { return x_; }
  double y() const { return y_; }
  Point point() const { return {x_, y_}; }

  const std::set<Edge *> &get_dependents() const { return dependents_; }

  void add_dependent(Edge *dependent) { /* [todo] Implement this function */ }
  void remove_dependent(Edge *dependent) { /* [todo] Implement this function */ }

  std::string to_json() const override;
  std::string to_svg() const override;
  BoundingBox get_bounding_box() const override;
};

#endif // VERTEX_H