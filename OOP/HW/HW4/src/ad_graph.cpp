#include "ad_graph.h"
#include <ranges>

namespace ad {

/**
 * @brief Destructor for the Graph class. Clears the graph and frees all allocated memory for nodes.
 * This ensures that there are no memory leaks when a Graph object goes out of scope.
 */
Graph::~Graph() { 
  clear(); 
}

/**
 * @brief Adds a node to the graph and returns its NodeId.
 * @param node The node to add.
 * @return The NodeId of the added node.
 */
NodeId Graph::push_node(detail::Node* node) {
  nodes_.push_back(node);
  return nodes_.size() - 1;
}

/**
 * @brief Creates an input node in the graph with the given value and returns its NodeId.
 * @param val The value of the input node.
 * @return The NodeId of the newly created input node.
 */
NodeId Graph::create_input(double val) {
  return push_node(new detail::InputNode(val));
}

/**
 * @brief Creates a constant node in the graph with the given value and returns its NodeId.
 * @param val The value of the constant node.
 * @return The NodeId of the newly created constant node.
 */
NodeId Graph::create_const(double val) {
  return push_node(new detail::ConstNode(val));
}

/**
 * @brief Adds two nodes in the graph and returns the NodeId of the result.
 * @param l The left operand node.
 * @param r The right operand node.
 * @return The NodeId of the newly created addition node.
 */
NodeId Graph::add(NodeId l, NodeId r) {
  return push_node(new detail::AddNode(nodes_[l], nodes_[r]));
}

/**
 * @brief Subtracts two nodes in the graph and returns the NodeId of the result.
 * @param l The left operand node.
 * @param r The right operand node.
 * @return The NodeId of the newly created subtraction node.
 */
NodeId Graph::sub(NodeId l, NodeId r) {
  return push_node(new detail::SubNode(nodes_[l], nodes_[r]));
}

// [todo]: Implement 
NodeId Graph::mul(NodeId l, NodeId r){
  return push_node(new detail::MulNode(nodes_[l], nodes_[r]));
}

// [todo]: Implement 
NodeId Graph::div(NodeId l, NodeId r){
  return push_node(new detail::DivNode(nodes_[l], nodes_[r]));

}

// [todo]: Implement 
NodeId Graph::sin(NodeId op){
  return push_node(new detail::SinNode(nodes_[op]));
}

// [todo]: Implement 
NodeId Graph::cos(NodeId op){
  return push_node(new detail::CosNode(nodes_[op]));
}

/**
 * @brief Computes the square root of a node in the graph and returns the NodeId of the result.
 * @param op The operand node.
 * @return The NodeId of the newly created square root node.
 */
NodeId Graph::sqrt(NodeId op) {
  return push_node(new detail::SqrtNode(nodes_[op]));
}

/**
 * @brief Computes the absolute value of a node in the graph and returns the NodeId of the result.
 * @param op The operand node.
 * @return The NodeId of the newly created absolute value node.
 */
NodeId Graph::abs(NodeId op) {
  return push_node(new detail::AbsNode(nodes_[op]));
}

/**
 * @brief Sets the value of a node in the graph.
 * @param id The NodeId of the node to set.
 * @param val The value to set for the node.
 */
void Graph::set_value(NodeId id, double val) {
  nodes_[id]->value = val;
}

/**
 * @brief Gets the value of a node in the graph.
 * @param id The NodeId of the node to get.
 * @return The value of the node.
 */
double Graph::get_value(NodeId id) const { 
  return nodes_[id]->value; 
}

/**
 * @brief Gets the adjoint of a node in the graph.
 * @param id The NodeId of the node to get.
 * @return The adjoint of the node.
 */
double Graph::get_adjoint(NodeId id) const { 
  return nodes_[id]->adjoint; 
}

/* * DESIGN NOTE: Implicit Topological Sorting
 * To evaluate a computational graph correctly, nodes must be processed in topological
 * order (a node is never evaluated before its dependencies). Because of C++'s
 * "Define-Before-Use" rule, whenever an operation like 'x + y' is called, 'x'
 * and 'y' must already exist. Since our graph arena strictly appends new nodes
 * to the back of the `nodes_` vector, a node is mathematically guaranteed to be
 * placed at an index greater than its operands. Therefore, the vector is ALREADY
 * in perfect topological order! 
 */

// [todo]: Implement 
void Graph::forward(){
  for (auto node : nodes_) {
    node->forward();
  }

}
// Hint: Iterate over `nodes_` and call `forward()` on each node.

// [todo]: Implement void Graph::backward(NodeId target)
// Hint: 
// 1. Iterate over `nodes_` and set `adjoint = 0.0` to reset from previous runs.
// 2. Set the target node's adjoint to 1.0.
// 3. Iterate over `nodes_` in reverse (e.g., using std::views::reverse(nodes_))
// and call `backward()`.
void Graph::backward(NodeId target){
  for (auto node : nodes_) {
    node->adjoint = 0.0;
  }
  nodes_[target]->adjoint = 1.0;
  for (auto node : std::views::reverse(nodes_)) {
    node->backward();
  }

}
// [todo]: Implement void Graph::clear()
// Hint: Iterate over `nodes_`, safely delete each pointer to prevent memory leaks,
// then clear() the vector.
void Graph::clear(){
  for (auto node : nodes_) {
    delete node;
  }
  nodes_.clear();
}
} // namespace ad