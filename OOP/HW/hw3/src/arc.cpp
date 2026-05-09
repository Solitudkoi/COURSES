#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "arc.h"
#include "point.h"
#include "vertex.h"
#include "json_constants.h"
#include "math_constants.h"
#include "svg_constants.h"

// Tolerance for checking equidistance of start and end points from center
const double EQUIDISTANCE_TOLERANCE = 1e-2;

/**
 * @brief Construct an Arc given center, start, and end vertices.
 *        Validates that start and end are equidistant from center.
 * @param id Unique identifier for the arc
 * @param center Pointer to the center Vertex
 * @param start Pointer to the start Vertex
 * @param end Pointer to the end Vertex
 */
Arc::Arc(EntityId id, Vertex *center, Vertex *start, Vertex *end)
    : Edge(id), c_(center), p1_(start), p2_(end) {
  if (c_ == nullptr || p1_ == nullptr || p2_ == nullptr)
    throw std::invalid_argument("Center/Start/End points cannot be null.");

  double r1 = std::hypot(p1_->x() - c_->x(), p1_->y() - c_->y());
  double r2 = std::hypot(p2_->x() - c_->x(), p2_->y() - c_->y());
  if (std::abs(r1 - r2) > EQUIDISTANCE_TOLERANCE)
    throw std::invalid_argument("Start and End points must be equidistant from Center.");
}

/**
 * @brief Generate JSON representation of the arc
 * @return JSON string for the arc
 */
std::string Arc::to_json() const {
  std::stringstream ss;
  // Since Arc has no floating point numbers to write, setting precision is not
  // necessary here, but we do it for consistency.
  ss << std::fixed << std::setprecision(JSON_PRECISION);

  ss << "{\"id\":" << id() << ",\"type\":\"Arc\""
     << ",\"center_id\":" << c_->id()
     << ",\"start_id\":" << p1_->id()
     << ",\"end_id\":" << p2_->id() << "}";

  return ss.str();
}

/**
 * @brief Generate SVG representation of the arc using SVG path element with arc command
 * @return SVG string for the arc
 */
std::string Arc::to_svg() const {
  // Calculate radius
  double r = std::hypot(p1_->x() - c_->x(), p1_->y() - c_->y());

  // Vector from center to start point
  double v1x = p1_->x() - c_->x();
  double v1y = p1_->y() - c_->y();
  // Vector from center to end point
  double v2x = p2_->x() - c_->x();
  double v2y = p2_->y() - c_->y();

  // The SVG arc path command does not use the center point directly, so there
  // are two possibilities for the arc between two points: the short path and
  // the long path. If cross_product is negative, then the arc angle is greater
  // than 180 degrees, and we set large_arc_flag = 1.
  double cross_product = v1x * v2y - v1y * v2x;
  int large_arc_flag = (cross_product < 0) ? 1 : 0;

  // Based on our arc assumption, we ALWAYS go CCW. In SVG, CCW is sweep_flag = 0.
  int sweep_flag = 0; 

  // Transform points to SVG coordinate system before outputting
  Point p1 = cartesian_to_svg(p1_->point());
  Point p2 = cartesian_to_svg(p2_->point());

  // Construct SVG path element for the arc
  std::stringstream ss;
  ss << std::fixed << std::setprecision(SVG_PRECISION);
  ss << "  <path d=\"M " << p1.x << " " << p1.y << " A " << r << " "
     << r << " 0 " << large_arc_flag << " " << sweep_flag << " "
     << p2.x << " " << p2.y << "\" stroke=\"black\" stroke-width=\""
     << SVG_LINE_WIDTH << "\" stroke-linecap=\"round\" fill=\"none\"/>\n";

  return ss.str();
}

/**
 * @brief Calculate the axis-aligned bounding box of the arc, accounting for
 *        the possibility that the arc may cross extremal points.
 * @return BoundingBox that contains the entire arc
 */
BoundingBox Arc::get_bounding_box() const {
  // Get coordinates
  double p1x = p1_->x();
  double p1y = p1_->y();
  double p2x = p2_->x();
  double p2y = p2_->y();
  double cx = c_->x();
  double cy = c_->y();

  // Initialize bounding box with start and end points
  BoundingBox bb = {
    std::min(cx, std::min(p1x, p2x)),
    std::max(cx, std::max(p1x, p2x)),
    std::min(cy, std::min(p1y, p2y)),
    std::max(cy, std::max(p1y, p2y))
  };

  // Get center, radius, and angles. Note that the angles 
  // are in the range [-pi, pi].
  double r = std::hypot(p1x - cx, p1y - cy);
  double start_angle = std::atan2(p1y - cy, p1x - cx);
  double end_angle = std::atan2(p2y - cy, p2x - cx);

  // Normalize angles for CCW travel, we want to ensure
  // that end_angle >= start_angle
  if (end_angle < start_angle) {
    end_angle += 2.0 * PI;
  }

  // Check if arc crosses extremal points (0, 90, 180, 270 degrees)
  // and expand the bounding box accordingly.
  auto in_range = [start_angle, end_angle](double angle) {
    if (angle < start_angle) {
      angle += 2.0 * PI;
    }
    return angle < end_angle;
  };
  if (in_range(0.0)) {            // Check for 0 radians (Right, +X)
    bb.max_x = std::max(bb.max_x, cx + r);
  }
  if (in_range(PI / 2.0)) {       // Check for PI/2 radians (Top, +Y)
    bb.max_y = std::max(bb.max_y, cy + r);
  }
  if (in_range(PI)) {             // Check for PI radians (Left, -X)
    bb.min_x = std::min(bb.min_x, cx - r);
  }
  if (in_range(3.0 * PI / 2.0)) { // Check for 3*PI/2 radians (Bottom, -Y)
    bb.min_y = std::min(bb.min_y, cy - r);
  }

  return bb;
}