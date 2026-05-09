#ifndef LINALG_LINEAR_SOLVER_H
#define LINALG_LINEAR_SOLVER_H

#include "matrix.h"
#include "vector.h"

namespace linalg {

/**
 * @brief Abstract base class for linear solvers. Defines the interface for solving linear systems
 * of the form Ax = b.
 */
class LinearSolver {
public:
  virtual ~LinearSolver() = default;
  virtual Vector solve(Matrix A, Vector b) = 0;
};

}  // namespace linalg

#endif  // LINALG_LINEAR_SOLVER_H
