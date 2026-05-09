#ifndef ARC_H
#define ARC_H

#include "edge.h"

/**
 * @brief Represents a circular arc defined by three vertices: center, start,
 *        and end, going counter-clockwise (CCW) from start to end.
 */
class Arc : public Edge {
private:
  Vertex *c_;
  Vertex *p1_;
  Vertex *p2_;

public:
  Arc(EntityId id, Vertex *center, Vertex *start, Vertex *end);

  // Accessors
  Vertex *center() const { return c_; }
  Vertex *p1() const { return p1_; }
  Vertex *p2() const { return p2_; }

  std::vector<Vertex *> get_dependencies() const override {
    return {c_, p1_, p2_};
  }

  std::string to_json() const override;
  std::string to_svg() const override;
  BoundingBox get_bounding_box() const override;
};

#endif // ARC_H