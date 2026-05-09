#ifndef LINE_H
#define LINE_H

#include "edge.h"

class Vertex;

/**
 * @brief Represents a straight line defined by two vertices (endpoints).
 */
class Line : public Edge {
private:
  Vertex *p1_;
  Vertex *p2_;

public:
  Line(EntityId id, Vertex *p1, Vertex *p2);

  Vertex *p1() const { return p1_; }
  Vertex *p2() const { return p2_; }

  std::vector<Vertex *> get_dependencies() const override {
    return {p1_, p2_};
  }

  std::string to_json() const override;
  std::string to_svg() const override;
  BoundingBox get_bounding_box() const override;
};

#endif // LINE_H
