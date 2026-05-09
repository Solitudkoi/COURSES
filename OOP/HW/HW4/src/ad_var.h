#pragma once
#include "ad_graph.h"

namespace ad {

/**
 * @brief The Var class represents a variable in the computational graph for automatic differentiation.
 * It provides a convenient interface for creating and manipulating nodes in the graph.
 */
class Var {
private:
  // Pointer to the computational graph that this variable belongs to
  Graph *graph_;
  // The NodeId of the node in the graph that this variable represents
  NodeId id_;

private:
  // Private constructor to create a Var from a Graph and NodeId
  Var(Graph *g, NodeId id);

public:
  // Factory method to create an input variable in the graph
  static Var Input(Graph &g, double val);

  // Methods to access and manipulate the variable's value and adjoint
  void set_value(double v);
  double value() const;
  double adjoint() const;
  
  // Method to perform backpropagation starting from this variable
  void backward();
  NodeId node_id() const;

  // The "Hidden Friend" idiom: Implemented inline for better Argument-Dependent Lookup (ADL)
  // This allows us to use operators like +, -, *, /, sin, cos, etc. directly on Var objects without
  // needing to qualify them with the namespace.
  friend Var operator+(const Var &a, const Var &b) { return {a.graph_, a.graph_->add(a.id_, b.id_)}; }
  friend Var operator-(const Var &a, const Var &b) { return {a.graph_, a.graph_->sub(a.id_, b.id_)}; }

  // [todo]: Implement friend Var operator*(const Var &a, const Var &b)
  friend Var operator*(const Var &a, const Var &b) { return {a.graph_, a.graph_->mul(a.id_, b.id_)}; }
  // [todo]: Implement friend Var operator/(const Var &a, const Var &b)
  friend Var operator/(const Var &a, const Var &b) { return {a.graph_, a.graph_->div(a.id_, b.id_)}; }

  // [todo]: Implement friend Var sin(const Var &a)
  friend Var sin(const Var &a) { return {a.graph_, a.graph_->sin(a.id_)}; }
  // [todo]: Implement friend Var cos(const Var &a)
  friend Var cos(const Var &a) { return {a.graph_, a.graph_->cos(a.id_)}; }

  friend Var sqrt(const Var &a) { return {a.graph_, a.graph_->sqrt(a.id_)}; }
  friend Var abs(const Var &a) { return {a.graph_, a.graph_->abs(a.id_)}; }
  friend Var pow2(const Var &a) { return {a.graph_, a.graph_->mul(a.id_, a.id_)}; }

  // Overloads for operations with double literals, allowing seamless integration of constants in expressions
  friend Var operator+(const Var &a, double b) { return a + Var(a.graph_, a.graph_->create_const(b)); }
  friend Var operator+(double a, const Var &b) { return Var(b.graph_, b.graph_->create_const(a)) + b; }
  friend Var operator-(const Var &a, double b) { return a - Var(a.graph_, a.graph_->create_const(b)); }
  friend Var operator-(double a, const Var &b) { return Var(b.graph_, b.graph_->create_const(a)) - b; }
  friend Var operator*(const Var &a, double b) { return a * Var(a.graph_, a.graph_->create_const(b)); }

  // [todo]: Implement friend Var operator*(double a, const Var &b)
  friend Var operator*(double a, const Var &b) { return Var(b.graph_, b.graph_->create_const(a)) * b; }
  // [todo]: Implement friend Var operator/(const Var &a, double b)
  friend Var operator/(const Var &a, double b) { return a / Var(a.graph_, a.graph_->create_const(b)); }
};

} // namespace ad
