#ifndef POINT_H
#define POINT_H

/**
 * @brief Represents a point in 2D space, plain struct for easy usage.
 */
struct Point { double x; double y; };

/**
 * @brief Converts Cartesian coordinates to SVG coordinates by inverting the Y-axis.
 * @param p The point in Cartesian coordinates.
 * @return The point in SVG coordinates.
 */
inline Point cartesian_to_svg(Point p) {
  // SVG Y axis is inverted compared to Cartesian Y axis
  return {p.x, -p.y};
}

#endif // POINT_H