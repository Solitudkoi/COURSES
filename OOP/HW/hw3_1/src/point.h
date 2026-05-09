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
inline Point cartesian_to_svg(Point p) {//这是一个内联函数，用于将笛卡尔坐标转换为SVG坐标。由于SVG的Y轴与笛卡尔坐标系的Y轴相反，所以需要将Y值取反。
  // SVG Y axis is inverted compared to Cartesian Y axis
  return {p.x, -p.y};
}

#endif // POINT_H