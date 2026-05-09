#include <iomanip>
#include <iostream>
#include <vector>
#include "point_line.h"

int main() {
  // Read points and lines data
  auto [points, lines] = read_data();

  // Print the formatted output
  print_table(points, lines);

  // Write the SVG output
  write_svg(points, lines);

  return 0;
}
