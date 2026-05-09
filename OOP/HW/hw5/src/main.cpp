#include <cmath>
#include "linear_algebra.h"
#include "nano_test.h"

using namespace linalg;

// --- Vector Tests ---
nano_test::Register TestVectorBasic("Vector_Basic", []() {
  Vector v = {1.0, 2.0, -3.0}; // Testing initializer_list
  
  nano_test::expect_eq(v.size(), 3);
  nano_test::expect_approx_eq(v[0], 1.0);
  nano_test::expect_approx_eq(v[2], -3.0);
  
  // Norms
  nano_test::expect_approx_eq(v.euclidean_norm(), std::sqrt(14.0));
  nano_test::expect_approx_eq(v.infinity_norm(), 3.0);
});

nano_test::Register TestVectorArithmetic("Vector_Arithmetic", []() {
  Vector v1 = {1.0, 2.0, 3.0};
  Vector v2 = {4.0, 5.0, 6.0};
  
  // Addition & Subtraction
  Vector sum = v1 + v2;
  nano_test::expect_approx_eq(sum[0], 5.0);
  nano_test::expect_approx_eq(sum[2], 9.0);
  
  Vector diff = v2 - v1;
  nano_test::expect_approx_eq(diff[1], 3.0);
  
  // Compound Assignment
  v1 += v2;
  nano_test::expect_approx_eq(v1[0], 5.0);
});

nano_test::Register TestVectorScalarOps("Vector_Scalar_Ops", []() {
  Vector v = {2.0, -4.0};
  
  // Unary minus (Member function)
  Vector neg_v = -v;
  nano_test::expect_approx_eq(neg_v[0], -2.0);
  
  // Scalar multiplication (Friends, Commutative)
  Vector scaled1 = v * 0.5;
  Vector scaled2 = 0.5 * v;
  nano_test::expect_approx_eq(scaled1[1], -2.0);
  nano_test::expect_approx_eq(scaled2[1], -2.0);
  
  // Scalar division
  Vector div = v / 2.0;
  nano_test::expect_approx_eq(div[0], 1.0);
});

nano_test::Register TestVectorDotProduct("Vector_Dot_Product", []() {
  Vector v1 = {1.0, 3.0, -5.0};
  Vector v2 = {4.0, -2.0, -1.0};
  
  // Dot product: (1*4) + (3*-2) + (-5*-1) = 4 - 6 + 5 = 3
  double dot = v1.dot(v2);
  nano_test::expect_approx_eq(dot, 3.0);
});

// --- Matrix Tests ---
nano_test::Register TestMatrixBasic("Matrix_Basic", []() {
  Matrix m(2, 3);
  nano_test::expect_eq(m.rows(), 2);
  nano_test::expect_eq(m.cols(), 3);
  
  // Testing C++23 operator[](int, int)
  m[0, 1] = 5.0;
  nano_test::expect_approx_eq(m[0, 1], 5.0);
});

nano_test::Register TestMatrixArithmetic("Matrix_Arithmetic", []() {
  Matrix m1(2, 2);
  m1[0, 0] = 1.0; m1[0, 1] = 2.0;
  m1[1, 0] = 3.0; m1[1, 1] = 4.0;
  
  Matrix m2(2, 2);
  m2[0, 0] = 2.0; m2[0, 1] = 0.0;
  m2[1, 0] = 1.0; m2[1, 1] = 2.0;
  
  Matrix sum = m1 + m2;
  nano_test::expect_approx_eq(sum[0, 0], 3.0);
  nano_test::expect_approx_eq(sum[1, 1], 6.0);
  
  Matrix scaled = m1 * 2.0;
  nano_test::expect_approx_eq(scaled[1, 0], 6.0);
});

nano_test::Register TestMatrixMultiplication("Matrix_Multiplication", []() {
  Matrix m(2, 2);
  m[0, 0] = 1.0; m[0, 1] = 2.0;
  m[1, 0] = 3.0; m[1, 1] = 4.0;
  
  // Matrix * Vector
  Vector v = {1.0, 2.0};
  Vector res = m * v; // [1(1)+2(2), 3(1)+4(2)] = [5, 11]
  nano_test::expect_approx_eq(res[0], 5.0);
  nano_test::expect_approx_eq(res[1], 11.0);
  
  // Matrix * Matrix
  Matrix identity(2, 2);
  identity[0, 0] = 1.0; identity[1, 1] = 1.0;
  Matrix m_id = m * identity;
  nano_test::expect_approx_eq(m_id[0, 0], 1.0);
  nano_test::expect_approx_eq(m_id[1, 1], 4.0);
});

// --- Gaussian Elimination Tests ---
nano_test::Register TestGaussianElimination("GaussianElimination_Solve", []() {
  // Solve the system:
  // 2x +  y = 5
  //  x -  y = 1
  // Expected solution: x = 2, y = 1
  Matrix A(2, 2);
  A[0, 0] = 2.0; A[0, 1] =  1.0;
  A[1, 0] = 1.0; A[1, 1] = -1.0;
  
  Vector b = {5.0, 1.0};
  
  GaussianElimination solver;
  Vector x = solver.solve(A, b);
  
  nano_test::expect_approx_eq(x[0], 2.0);
  nano_test::expect_approx_eq(x[1], 1.0);
});

// --- Exception Tests ---
nano_test::Register TestVectorExceptions("Vector_Exceptions", []() {
  Vector v2 = {1.0, 2.0};
  Vector v3 = {1.0, 2.0, 3.0};

  // Vector Addition/Subtraction mismatch
  nano_test::expect_throw<std::invalid_argument>([&]() {
    v2 + v3;
  });
  nano_test::expect_throw<std::invalid_argument>([&]() {
    v2 - v3;
  });
  
  // Dot product mismatch
  nano_test::expect_throw<std::invalid_argument>([&]() {
    v2.dot(v3);
  });
});

nano_test::Register TestMatrixExceptions("Matrix_Exceptions", []() {
  Matrix m2x2(2, 2);
  Matrix m2x3(2, 3);
  Vector v3 = {1.0, 2.0, 3.0};

  // Matrix Addition/Subtraction mismatch
  nano_test::expect_throw<std::invalid_argument>([&]() {
    m2x2 + m2x3;
  });
  nano_test::expect_throw<std::invalid_argument>([&]() {
    m2x2 - m2x3;
  });

  // Matrix * Vector dimension mismatch (2 cols vs 3 elements)
  nano_test::expect_throw<std::invalid_argument>([&]() {
    m2x2 * v3;
  });

  // Matrix * Matrix dimension mismatch (m2x3 * m2x2 -> 3 cols != 2 rows)
  nano_test::expect_throw<std::invalid_argument>([&]() {
    m2x3 * m2x2;
  });
});

nano_test::Register TestSolverExceptions("Solver_Exceptions", []() {
  GaussianElimination solver;
  Matrix non_square(2, 3);
  Matrix square(2, 2);
  Vector b_mismatch = {1.0, 2.0, 3.0};

  // Matrix must be square
  nano_test::expect_throw<std::invalid_argument>([&]() {
    solver.solve(non_square, b_mismatch);
  });

  // Dimension mismatch for A and b (A is 2x2, b has 3 elements)
  nano_test::expect_throw<std::invalid_argument>([&]() {
    solver.solve(square, b_mismatch);
  });
});

int main() {
  return nano_test::Registry::instance().run();
}