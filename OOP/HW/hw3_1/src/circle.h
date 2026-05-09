#ifndef CIRCLE_H
#define CIRCLE_H

#include "edge.h"

class Vertex;

/**
 * @brief Represents a circle defined by a center vertex and a radius.
 */
class Circle : public Edge {
private:
  Vertex *c_;
  double radius_;

public:
  Circle(EntityId id, Vertex *center, double radius);

  Vertex *center() const { return c_; }
  double radius() const { return radius_; }

  std::vector<Vertex *> get_dependencies() const override {
    return {c_};
  }

  std::string to_json() const override;
  std::string to_svg() const override;
  BoundingBox get_bounding_box() const override;
};

#endif // CIRCLE_H
