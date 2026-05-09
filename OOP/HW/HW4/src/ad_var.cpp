#include "ad_var.h"

namespace ad {

/**
 * @brief Private constructor for the Var class.
 * Initializes a Var with a pointer to the graph and a NodeId.
 */
Var::Var(Graph *g, NodeId id) : graph_(g), id_(id) {}

/**
 * @brief Factory method to create an input variable in the graph.
 * @param g The graph to which the variable belongs.
 * @param val The initial value of the input variable.
 * @return A Var object representing the input variable.
 */
Var Var::Input(Graph &g, double val) {
  return Var(&g, g.create_input(val));
}

/**
 * @brief Sets the value of the variable in the graph.
 * @param v The value to set for the variable.
 */
void Var::set_value(double v) {
  graph_->set_value(id_, v);
}

/**
 * @brief Gets the value of the variable from the graph.
 * @return The value of the variable.
 */
double Var::value() const {
  return graph_->get_value(id_);
}

/**
 * @brief Gets the adjoint of the variable from the graph.
 * @return The adjoint of the variable.
 */
double Var::adjoint() const {
  return graph_->get_adjoint(id_);
}

// [todo]: Implement void Var::backward()
// Hint: Just route the call to graph_->backward(id_)
void Var::backward() {
  graph_->backward(id_);
}

/**
 * @brief Gets the NodeId of the variable in the graph.
 * @return The NodeId of the variable.
 */
NodeId Var::node_id() const {
  return id_;
}

} // namespace ad