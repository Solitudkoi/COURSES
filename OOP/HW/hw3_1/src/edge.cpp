#include "edge.h"
#include "vertex.h"

/**
 * @brief Registers this edge with all its dependent vertices by using
 *        the "Template Method" pattern, i.e., calling the virtual
 *        get_dependencies() method to get the list of vertices and then
 *        registering this edge with each vertex.
 */
void Edge::register_with_dependencies() {//这是一个模板方法，调用虚函数get_dependencies()来获取依赖的顶点列表，然后将当前边注册到每个顶点的依赖列表中
  for (Vertex *v : get_dependencies()) {
    v->add_dependent(this);
  }
}

/**
 * @brief Unregisters this edge from all its dependent vertices by using
 *        the "Template Method" pattern, i.e., calling the virtual
 *        get_dependencies() method to get the list of vertices and then
 *        unregistering this edge from each vertex.
 */
void Edge::unregister_from_dependencies() {
  for (Vertex *v : get_dependencies()) {
    v->remove_dependent(this);
  }
}