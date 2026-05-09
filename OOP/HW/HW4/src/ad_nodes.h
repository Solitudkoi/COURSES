#pragma once
#include <cmath>

namespace ad {

/**
 * @brief NodeId is a type alias for an integer that uniquely identifies a node in the
 * computational graph. 
 */
using NodeId = int;

/**
 * @brief The detail namespace contains the internal implementation details of the computational
 * graph, including the definitions of various node types (e.g., input, constant, binary operations).
 * These classes are not intended to be used directly by users of the library, but rather serve as
 * the building blocks for the Graph and Var classes that provide the public interface for automatic
 * differentiation.
 */
namespace detail {

/**
 * @brief The base class for all nodes in the computational graph. It defines the interface for
 * forward and backward passes, as well as storage for the value and adjoint of the node. 
 */
struct Node {
  // [todo]: Implement a virtual destructor for a polymorphic base class to ensure proper
  // cleanup of derived classes
    virtual ~Node() = default;

  // [todo]: Declare pure virtual void forward(). Each derived node type will implement this
  // method to compute its value based on its specific operation.
    virtual void forward() = 0;

  // [todo]: Declare pure virtual void backward(). Each derived node type will implement this
  // method to propogate its adjoint to its input nodes based on the chain rule of differentiation.
    virtual void backward() = 0;
  // Storage for the value and adjoint of the node.
  double value = 0.0;
  double adjoint = 0.0;
};

/**
 * @brief Node for input values in the computational graph. This node type represents variables that
 * are provided as input to the graph. The forward and backward methods for this node type do not
 * perform any computations, as the value is set directly and the adjoint is accumulated during
 * backprop agation based on the contributions from downstream nodes.
 */
struct InputNode : Node {
  InputNode(double v) { value = v; }
  void forward() override {}
  void backward() override {}
};

/**
 * @brief Node for constant values in the computational graph. This node type represents fixed values
 * that do not change during the computation. Similar to InputNode, the forward and backward methods
 * do not perform any computations, as the value is set directly and the adjoint is accumulated
 * during backpropagation based on the contributions from downstream nodes.
 */
struct ConstNode : Node {
  ConstNode(double v) { value = v; }
  void forward() override {}
  void backward() override {}
};

/**
 * @brief Base class for binary operations in the computational graph. This class serves as a common
 * base for specific binary operation nodes (e.g., addition, subtraction, multiplication, division)
 * and provides storage for the left and right operand nodes.
 */
struct BinaryNode : Node {
  Node *l, *r;
  BinaryNode(Node *l, Node *r) : l(l), r(r) {}
};

/**
 * @brief Node for addition operations in the computational graph.
 */
struct AddNode : BinaryNode {
  using BinaryNode::BinaryNode;
  void forward() override { value = l->value + r->value; }
  void backward() override { l->adjoint += adjoint; r->adjoint += adjoint; }
};

/**
 * @brief Node for subtraction operations in the computational graph.
 */
struct SubNode : BinaryNode {
  using BinaryNode::BinaryNode;
  void forward() override { value = l->value - r->value; }
  void backward() override { l->adjoint += adjoint; r->adjoint -= adjoint; }
};

// [todo]: Implement
struct MulNode : BinaryNode {
  using BinaryNode::BinaryNode;
  void forward() override { value = l->value * r->value; }
  void backward() override {
    l->adjoint += adjoint * r->value;
    r->adjoint += adjoint * l->value;
  }
};

// [todo]: Implement
struct DivNode : BinaryNode {
  using BinaryNode::BinaryNode;
  void forward() override { value = l->value / r->value; }
  void backward() override {
    l->adjoint += adjoint / r->value;
    r->adjoint -= adjoint * l->value / (r->value * r->value);
  }
};

/**
 * @brief Base class for unary operations in the computational graph. This class serves as a common
 * base for specific unary operation nodes (e.g., sine, cosine, square root, absolute value) and
 * provides storage for the operand node.
 */
struct UnaryNode : Node {
  Node *op;
  UnaryNode(Node *op) : op(op) {}
};

// [todo]: Implement
struct SinNode : UnaryNode {
  using UnaryNode::UnaryNode;
  void forward() override { value = std::sin(op->value); }
  void backward() override { op->adjoint += adjoint * std::cos(op->value); }
};

// [todo]: Implement
struct CosNode : UnaryNode {
  using UnaryNode::UnaryNode;
  void forward() override { value = std::cos(op->value); }
  void backward() override { op->adjoint -= adjoint * std::sin(op->value); }
};

/**
 * @brief Node for square root operations in the computational graph.
 */
struct SqrtNode : UnaryNode {
  using UnaryNode::UnaryNode;
  void forward() override { value = std::sqrt(op->value); }
  void backward() override { op->adjoint += adjoint * 0.5 / value; }
};

/**
 * @brief Node for absolute value operations in the computational graph.
 */
struct AbsNode : UnaryNode {
  using UnaryNode::UnaryNode;
  void forward() override { value = std::abs(op->value); }
  void backward() override { op->adjoint += adjoint * ((op->value < 0) ? -1.0 : 1.0); }
};

} // namespace detail
} // namespace ad