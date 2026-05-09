#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "line.h"
#include "point.h"
#include "vertex.h"
#include "json_constants.h"
#include "svg_constants.h"

Line::Line(EntityId id, Vertex *p1, Vertex *p2)
    : Edge(id), p1_(p1), p2_(p2) {
  if (p1_ == nullptr || p2_ == nullptr)
    throw std::invalid_argument("Endpoints cannot be null.");
}

std::string Line::to_json() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(JSON_PRECISION);
  ss << "{\"id\":" << id() << ",\"type\":\"Line\""
     << ",\"p1_id\":" << p1_->id()
     << ",\"p2_id\":" << p2_->id() << "}";
  return ss.str();
}

std::string Line::to_svg() const {
  Point sp1 = cartesian_to_svg(p1_->point());
  Point sp2 = cartesian_to_svg(p2_->point());
  std::stringstream ss;
  ss << std::fixed << std::setprecision(SVG_PRECISION);
  ss << "  <line x1=\"" << sp1.x << "\" y1=\"" << sp1.y
     << "\" x2=\"" << sp2.x << "\" y2=\"" << sp2.y
     << "\" stroke=\"black\" stroke-width=\"" << SVG_LINE_WIDTH
     << "\" stroke-linecap=\"round\"/>\n";
  return ss.str();
}

BoundingBox Line::get_bounding_box() const {
  double x1 = p1_->x(), y1 = p1_->y();
  double x2 = p2_->x(), y2 = p2_->y();
  return {std::min(x1, x2), std::max(x1, x2),
          std::min(y1, y2), std::max(y1, y2)};
}
