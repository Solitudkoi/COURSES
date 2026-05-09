#include <cmath>
#include <iostream>
#include "math_constants.h"
#include "sketch.h"

int main() {
  Sketch sketch;

  double r = 200;

  Vertex *p_center = sketch.add_vertex(0, 0);
  Circle *circle = sketch.add_circle(p_center, r);

  double l1 = 60, l2 = 120, l3 = 200;
  double r1 = 25;

  for (int i = 0; i < 4; ++i) {
    double rad = PI / 2 * i;
    Vertex *p1 = sketch.add_vertex(l1 * cos(rad), l1 * sin(rad));
    Vertex *p2 = sketch.add_vertex(l2 * cos(rad), l2 * sin(rad));
    Vertex *p3 = sketch.add_vertex(l3 * cos(rad), l3 * sin(rad));
    Vertex *pa = sketch.add_vertex((p1->x() + p2->x()) / 2 - r1 * sin(rad),
                                   (p1->y() + p2->y()) / 2 + r1 * cos(rad));
    sketch.add_line(p_center, p1);
    sketch.add_arc(pa, p2, p1);
    sketch.add_line(p2, p3);
  }

  sketch.save_to_json("sample.json");
  sketch.save_to_svg("sample.svg");

  sketch.delete_entity(22);
  sketch.delete_entity(16);

  sketch.save_to_json("sample_after_deletion.json");
  sketch.save_to_svg("sample_after_deletion.svg");

  return 0;
}
