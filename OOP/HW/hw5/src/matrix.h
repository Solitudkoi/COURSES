#ifndef LINALG_MATRIX_H
#define LINALG_MATRIX_H

#include <iostream>
#include "vector.h"

namespace linalg {

/**
 * @brief A simple matrix class for linear algebra operations.
 */
class Matrix {
private:
  // Number of rows, number of columns, and pointer to the data array (row-major order).
  int rows_;
  int cols_;
  double *data_;

public:
  // Constructors
  Matrix(int rows, int cols);

  // Rule of Three
  ~Matrix();
  Matrix(const Matrix &other);
  Matrix &operator=(const Matrix &other);

  // Accessors
  int rows() const { return rows_; }
  int cols() const { return cols_; }
  double &operator[](int r, int c) { return data_[r * cols_ + c]; }
  const double &operator[](int r, int c) const { return data_[r * cols_ + c]; }

  // Methods
  void swap_rows(int r1, int r2);
  void scale_row(int r, double scalar);
  void add_scaled_row(int src_row, int dest_row, double scalar);
  Matrix transpose() const;

  // Unary Operator
  Matrix operator-() const;

  // Assignment Operators
  Matrix &operator+=(const Matrix &other);
  Matrix &operator-=(const Matrix &other);
  Matrix &operator*=(double s);
  Matrix &operator/=(double s);
};

// Symmetric Binary Operators
Matrix operator+(const Matrix &lhs, const Matrix &rhs);
Matrix operator-(const Matrix &lhs, const Matrix &rhs);
Matrix operator*(const Matrix &m, double s);
Matrix operator*(double s, const Matrix &m);
Matrix operator/(const Matrix &m, double s);
Vector operator*(const Matrix &m, const Vector &v);
Matrix operator*(const Matrix &A, const Matrix &B);
std::ostream &operator<<(std::ostream &os, const Matrix &m);

}  // namespace linalg

#endif  // LINALG_MATRIX_H
