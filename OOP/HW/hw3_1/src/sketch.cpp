#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include "sketch.h"
#include "svg_constants.h"

/**
 * @brief Delete all heap-allocated Vertex and Edge objects
 */
void Sketch::clear_all() {
  // Delete edges first
  for (auto [_, ptr] : edges_) {
    delete ptr;
  }
  edges_.clear();

  // Then delete vertices
  for (auto [_, ptr] : vertices_) {
    delete ptr;
  }
  vertices_.clear();
}

/**
 * @brief Add a new Vertex to the sketch
 * @param x X coordinate
 * @param y Y coordinate
 * @return Pointer to the newly created Vertex
 */
Vertex *Sketch::add_vertex(double x, double y) {
  EntityId new_id = next_entity_id_++;
  Vertex *p = new Vertex(new_id, x, y);
  vertices_[p->id()] = p; // Store in vertices map
  return p;
}

/**
 * @brief Add a new Line edge to the sketch
 * @param p1 Pointer to the first endpoint Vertex
 * @param p2 Pointer to the second endpoint Vertex
 * @return Pointer to the newly created Line
 */
Line *Sketch::add_line(Vertex *p1, Vertex *p2) {
  EntityId new_id = next_entity_id_++;
  Line *l = new Line(new_id, p1, p2);
  l->register_with_dependencies();
  edges_[l->id()] = l;
  return l;
}

Circle *Sketch::add_circle(Vertex *center, double radius) {
  EntityId new_id = next_entity_id_++;
  Circle *c = new Circle(new_id, center, radius);
  c->register_with_dependencies();
  edges_[c->id()] = c;
  return c;
}

Arc *Sketch::add_arc(Vertex *center, Vertex *start, Vertex *end) {
  EntityId new_id = next_entity_id_++;
  Arc *a = new Arc(new_id, center, start, end);
  a->register_with_dependencies();
  edges_[a->id()] = a;
  return a;
}

/**
 * @brief Delete an entity (Vertex or Edge) by its ID, along with any dependent
 *        entities (such as edges connected to a deleted vertex), and any orphaned
 *        vertices that result from edge deletions.
 * @param id The unique identifier of the entity to delete
 */
void Sketch::delete_entity(EntityId id) {
  // --- Sets to track entities to delete ---
  std::set<EntityId> vertices_to_delete;
  std::set<EntityId> edges_to_delete;

  if (vertices_.contains(id)) { // It is a vertex
    vertices_to_delete.insert(id);
    auto const &p = vertices_.at(id);
    // Edges dependent on this vertex must also be deleted
    for (Edge *dependent_edge : p->get_dependents()) {
      edges_to_delete.insert(dependent_edge->id());
    }
  } else if (edges_.contains(id)) { // It is an edge
    edges_to_delete.insert(id);
  } else {
    throw std::runtime_error("Entity ID not found: " + std::to_string(id));
  }

  // --- Gather vertices connected to edges being deleted ---
  std::set<Vertex *> potential_orphans;
  for (EntityId edge_id : edges_to_delete) {
    Edge *e = edges_.at(edge_id);
    for (Vertex *p : e->get_dependencies()) {
      potential_orphans.insert(p);
    }
  }

  // --- Identify orphaned vertices, i.e., whose (edge) dependents are all being deleted ---
  for (Vertex *p : potential_orphans) {
    bool is_orphan = true;
    for (Edge *dependent : p->get_dependents()) {
      if (!edges_to_delete.contains(dependent->id())) {
        is_orphan = false;
        break;
      }
    }
    // orphaned vertex should also be deleted
    if (is_orphan) {
      vertices_to_delete.insert(p->id());
    }
  }

  // --- Perform actual deletions: edges first, then vertices ---
  for (EntityId edge_id : edges_to_delete) {
    Edge *e = edges_.at(edge_id);
    e->unregister_from_dependencies();
    edges_.erase(edge_id);
    delete e;
  }

  for (EntityId vertex_id : vertices_to_delete) {
    Vertex *v = vertices_.at(vertex_id);
    vertices_.erase(vertex_id);
    delete v;
  }

}

// Implement "to_json" method
std::string Sketch::to_json() const {
  std::stringstream ss;
  ss << "{\n\"vertices\": [\n";

  bool first = true;
  for (auto const &[_, v] : vertices_) {
    if (!first) ss << ",\n";
    ss << "    " << v->to_json();
    first = false;
  }
  ss << "\n  ],\n\"edges\": [\n";

  first = true;
  for (auto const &[_, e] : edges_) {
    if (!first) ss << ",\n";
    ss << "    " << e->to_json();
    first = false;
  }
  ss << "\n  ]\n}";

  return ss.str();
}

/**
 * @brief Print the current state of the sketch to the console, in JSON format
 */
void Sketch::print_state() const {
  std::cout << to_json() << std::endl;
}

/**
 * @brief Save the current state of the sketch to a JSON file
 * @param filepath Path to the output JSON file
 */
void Sketch::save_to_json(const std::string &filepath) const {
  std::ofstream file(filepath);
  if (!file.is_open())
    throw std::runtime_error("Could not open file: " + filepath);

  file << to_json();
}

/**
 * @brief Save the current sketch as an SVG file for visualization
 * @param filepath Path to the output SVG file
 */
void Sketch::save_to_svg(const std::string &filepath) const {
  std::ofstream file(filepath);
  if (!file.is_open())
    throw std::runtime_error("Could not open file: " + filepath);

  // --- Calculate Bounding Box ---
  BoundingBox master_bb;
  if (vertices_.empty() && edges_.empty()) {
    master_bb = {0.0, 0.0, 0.0, 0.0}; // Default box for empty sketch
  } else {
    // Expand bounding box for all vertices
    for (auto const &[_, v] : vertices_) {
      master_bb.merge(v->get_bounding_box());
    }
    // Expand bounding box for all edges
    for (auto const &[_, e] : edges_) {
      master_bb.merge(e->get_bounding_box());
    }
  }

  // --- Compute SVG viewBox parameters ---
  Point upper_left_svg = cartesian_to_svg({master_bb.min_x, master_bb.max_y});
  double viewbox_x = upper_left_svg.x - SVG_PADDING;
  double viewbox_y = upper_left_svg.y - SVG_PADDING;
  double viewbox_width = (master_bb.max_x - master_bb.min_x) + 2 * SVG_PADDING;
  double viewbox_height = (master_bb.max_y - master_bb.min_y) + 2 * SVG_PADDING;

  // --- Write SVG Header ---
  file << std::fixed << std::setprecision(SVG_PRECISION);
  file << "<svg width=\"" << viewbox_width << "\" height=\"" << viewbox_height
       << "\" viewBox=\"" << viewbox_x << " " << viewbox_y << " "
       << viewbox_width << " " << viewbox_height
       << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

  // --- Write SVG Edges (Lines, Circles, Arcs) ---
  for (auto const &[_, e] : edges_) {
    file << e->to_svg();
  }

  // --- Write SVG Vertices ---
  for (auto const &[_, v] : vertices_) {
    file << v->to_svg();
  }

  // --- Write SVG Footer ---
  file << "</svg>\n";
}