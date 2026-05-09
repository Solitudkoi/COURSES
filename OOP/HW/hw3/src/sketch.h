#ifndef SKETCH_H
#define SKETCH_H

#include <map>
#include <string>
#include <vector>
#include "arc.h"
#include "circle.h"
#include "line.h"
#include "vertex.h"

/**
 * @brief The Sketch class manages a collection of geometric entities, including
 *        vertices and edges (lines, circles, arcs). It provides functionality to
 *        add, delete, serialize (to JSON), and visualize these entities (to SVG).
 */
class Sketch {
private:
  // Next available unique entity ID
  EntityId next_entity_id_;
  // Map of entity IDs to their corresponding Vertex and Edge pointers
  std::map<EntityId, Vertex *> vertices_;
  std::map<EntityId, Edge *> edges_;

  // Helper function to delete all heap-allocated objects
  void clear_all();

public:
  // Constructor and Destructor
  Sketch() : next_entity_id_(0) {}
  ~Sketch() { clear_all(); }

  // Disable copy constructor and assignment to prevent shallow copies
  Sketch(const Sketch &) = delete;
  Sketch &operator=(const Sketch &) = delete;

  // Methods to add entities
  Vertex *add_vertex(double x, double y);
  Line *add_line(Vertex *p1, Vertex *p2);
  // [todo] Declare "add_circle" method
  // [todo] Declare "add_arc" method

  // Method to delete an entity by ID
  void delete_entity(EntityId id);

  // Serialization to JSON
  std::string to_json() const;
  void print_state() const;
  void save_to_json(const std::string &filepath) const;

  // Export to SVG for visualization
  void save_to_svg(const std::string &filepath) const;
};

#endif // SKETCH_H