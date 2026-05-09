#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "gaussian_elimination.h"

namespace linalg {

/**
 * @brief Solves the linear system Ax = b using Gaussian elimination with partial pivoting.
 */
Vector GaussianElimination::solve(Matrix A, Vector b) {
  constexpr double EPSILON = 1e-11;

  // Step 0: Check dimensions
  int n = A.rows();
  if (n != A.cols())
    throw std::invalid_argument("Matrix must be square.");
  if (n != b.size())
    throw std::invalid_argument("Dimension mismatch for A and b.");

  // Step 1: Forward elimination with partial pivoting
  for (int i = 0; i < n; ++i) {
    // Partial pivoting: find the row with the largest absolute value in column i
    int pivot = i;
    for (int j = i + 1; j < n; ++j) {
      if (std::abs(A[j, i]) > std::abs(A[pivot, i]))
        pivot = j;
    }
    // Swap the current row with the pivot row in both A and b
    A.swap_rows(i, pivot);
    std::swap(b[i], b[pivot]);

    // Check for singularity
    if (std::abs(A[i, i]) < EPSILON) {
      std::clog << "WARNING: A is singular!" << std::endl;
      continue;
    }

    // Eliminate entries below the pivot
    for (int j = i + 1; j < n; ++j) {
      double factor = -A[j, i] / A[i, i];
      A.add_scaled_row(i, j, factor);
      b[j] += b[i] * factor;
    }
  }

  // Step 2: Back substitution
  Vector x(n);
  for (int i = n - 1; i >= 0; --i) {
    // Compute the sum of A[i, j] * x[j] for j > i
    double sum = 0;
    for (int j = i + 1; j < n; ++j)
      sum += A[i, j] * x[j];

    // Solve for x[i]
    if (std::abs(A[i, i]) >= EPSILON) {
      x[i] = (b[i] - sum) / A[i, i];
    } else {
      x[i] = 0;
    }
  }

  return x;
}

}  // namespace linalg
