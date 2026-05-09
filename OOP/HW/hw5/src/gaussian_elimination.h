#ifndef LINALG_GAUSSIAN_ELIMINATION_H
#define LINALG_GAUSSIAN_ELIMINATION_H

#include "linear_solver.h"
#include "matrix.h"
#include "vector.h"

namespace linalg {

/**
 * @brief A concrete implementation of the LinearSolver interface using Gaussian elimination.
 * This class provides a method to solve linear systems of the form Ax = b by performing
 * Gaussian elimination with partial pivoting.
 */
class GaussianElimination : public LinearSolver {
public:
  Vector solve(Matrix A, Vector b) override;
};

}  // namespace linalg

#endif  // LINALG_GAUSSIAN_ELIMINATION_H