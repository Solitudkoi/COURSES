#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <algorithm>
#include <limits>

/**
 * @brief Represents an axis-aligned bounding box in 2D space, defined by
 *        its minimum and maximum x and y coordinates. Provides a method to
 *        merge with another bounding box, expanding its extents as necessary.
 */
struct BoundingBox {
  double min_x = std::numeric_limits<double>::max();  //最小x坐标
  double max_x = std::numeric_limits<double>::lowest();
  double min_y = std::numeric_limits<double>::max();
  double max_y = std::numeric_limits<double>::lowest();

  void merge(const BoundingBox& other) {  //合并另一个边界框，更新当前边界框的范围
    min_x = std::min(min_x, other.min_x);
    max_x = std::max(max_x, other.max_x);
    min_y = std::min(min_y, other.min_y);
    max_y = std::max(max_y, other.max_y);
  }
};

#endif // BOUNDING_BOX_H