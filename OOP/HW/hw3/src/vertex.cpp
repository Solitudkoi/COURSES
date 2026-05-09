#include <iomanip>
#include <sstream>
#include "json_constants.h"
#include "svg_constants.h"
#include "vertex.h"

/**
 * @brief Generate the JSON representation of the vertex
 * @return JSON string for the vertex
 */
std::string Vertex::to_json() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(JSON_PRECISION);

  ss << "{\"id\":" << id() << ",\"type\":\"Vertex\""
     << ",\"x\":" << x_ << ",\"y\":" << y_ << "}";

  return ss.str();
}

/**
 * @brief Generate the SVG representation of the vertex as a small circle
 * @return SVG string for the vertex
 */
std::string Vertex::to_svg() const {
  // Transform to SVG coordinate system before outputting
  Point p = cartesian_to_svg(point());

  // Create SVG circle element for the vertex
  std::stringstream ss;
  ss << std::fixed << std::setprecision(SVG_PRECISION);
  ss << "  <circle cx=\"" << p.x << "\" cy=\"" << p.y
     << "\" r=\"" << SVG_POINT_RADIUS
     << "\" fill=\"black\"/>\n";
  return ss.str();
}

/**
 * @brief Get the bounding box of the vertex (a point), which is just the point itself
 * @return BoundingBox object representing the vertex's bounding box
 */
BoundingBox Vertex::get_bounding_box() const {
  return { x_, x_, y_, y_ };
}