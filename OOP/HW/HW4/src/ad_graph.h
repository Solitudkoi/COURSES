#pragma once
#include <vector>
#include "ad_nodes.h"

namespace ad {

/**
 * @brief The Graph class represents a computational graph for automatic differentiation.
 * It manages the nodes in the graph, performs forward and backward propagation, and handles
 * memory management for the nodes.
 */
class Graph {
private:
  // Storage for all nodes in the graph. The index in the vector serves as the NodeId.
  std::vector<detail::Node*> nodes_;

private:
  // Helper method to add a node to the graph and return its ID
  NodeId push_node(detail::Node* node); 

public:
  ~Graph();

  // Node creation methods
  NodeId create_input(double val);
  NodeId create_const(double val);
  
  // Binary operations
  NodeId add(NodeId l, NodeId r);
  NodeId sub(NodeId l, NodeId r);
  NodeId mul(NodeId l, NodeId r);
  NodeId div(NodeId l, NodeId r);
  
  // Unary operations
  NodeId sin(NodeId op);
  NodeId cos(NodeId op);
  NodeId sqrt(NodeId op);
  NodeId abs(NodeId op);

  // Value and adjoint access
  void set_value(NodeId id, double val);
  double get_value(NodeId id) const;
  double get_adjoint(NodeId id) const;

  // Graph execution methods
  void forward();
  void backward(NodeId target);

  // Clear the graph and free memory
  void clear();
};

} // namespace ad