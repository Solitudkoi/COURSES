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
class Entity {  //这是一个抽象类，不能直接实例化，必须通过派生类来实现具体的几何实体
private:
  EntityId id_;

public: 
  Entity(EntityId id) : id_(id) {}
  virtual ~Entity() = default; // Virtual destructor is important for polymorphism
  EntityId id() const { return id_; }

  virtual std::string to_json() const = 0;  //设置为纯虚函数，要求派生类必须实现这个函数来返回实体的JSON表示
  virtual std::string to_svg() const = 0;     //设置为纯虚函数，要求派生类必须实现这个函数来返回实体的SVG表示
  virtual BoundingBox get_bounding_box() const = 0;   //设置为纯虚函数，要求派生类必须实现这个函数来返回实体的边界框
};

#endif // ENTITY_H