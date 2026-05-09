#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "bounding_box.h"

// Use specific type aliases for clarity
using EntityId = int;

/**
 * @brief Abstract base class for all geometric entities in the sketch.
 *        Each entity has a unique identifier and must implement methods for
 *        JSON serialization, SVG representation, and bounding box calculation.
 */
class Entity {
private:
  EntityId id_;

public:
  Entity(EntityId id) : id_(id) {}
  virtual ~Entity() = default; // Virtual destructor is important for polymorphism
  EntityId id() const { return id_; }

  virtual std::string to_json() const = 0;
  virtual std::string to_svg() const = 0;
  virtual BoundingBox get_bounding_box() const = 0;
};

#endif // ENTITY_H