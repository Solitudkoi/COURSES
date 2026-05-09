#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "circle.h"
#include "point.h"
#include "vertex.h"
#include "json_constants.h"
#include "svg_constants.h"

Circle::Circle(EntityId id, Vertex *center, double radius)
    : Edge(id), c_(center), radius_(radius) {
  if (c_ == nullptr)
    throw std::invalid_argument("Center point cannot be null.");
  if (radius_ <= 0)
    throw std::invalid_argument("Radius must be positive.");
}

std::string Circle::to_json() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(JSON_PRECISION);
  ss << "{\"id\":" << id() << ",\"type\":\"Circle\""
     << ",\"center_id\":" << c_->id()
     << ",\"radius\":" << radius_ << "}";
  return ss.str();
}

std::string Circle::to_svg() const {
  Point center = cartesian_to_svg(c_->point());
  std::stringstream ss;
  ss << std::fixed << std::setprecision(SVG_PRECISION);
  ss << "  <circle cx=\"" << center.x << "\" cy=\"" << center.y
     << "\" r=\"" << radius_
     << "\" stroke=\"black\" stroke-width=\"" << SVG_LINE_WIDTH
     << "\" fill=\"none\"/>\n";
  return ss.str();
}

BoundingBox Circle::get_bounding_box() const {
  double cx = c_->x();
  double cy = c_->y();
  return {cx - radius_, cx + radius_, cy - radius_, cy + radius_};
}
