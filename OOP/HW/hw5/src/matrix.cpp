#include <algorithm>
#include <stdexcept>
#include "matrix.h"

namespace linalg {

/**
 * @brief Constructs a Matrix of the given dimensions, initializing all elements to 0.0.
 */
Matrix::Matrix(int rows, int cols)
    : rows_(rows), cols_(cols), data_(new double[rows * cols]) {
  for (int i = 0; i < rows_ * cols_; ++i)
    data_[i] = 0.0;
}

/**
 * @brief Destructor.
 */
Matrix::~Matrix() {
  delete[] data_;
}

/**
 * @brief Copy constructor for deep copying another Matrix.
 */
Matrix::Matrix(const Matrix &other)
    : rows_(other.rows_), cols_(other.cols_), data_(new double[other.rows_ * other.cols_]) {
  for (int i = 0; i < rows_ * cols_; ++i)
    data_[i] = other.data_[i];
}

/**
 * @brief Copy assignment operator for deep copying another Matrix.
 */
Matrix &Matrix::operator=(const Matrix &other) {
  if (this != &other) {
    delete[] data_;
    rows_ = other.rows_;
    cols_ = other.cols_;
    data_ = new double[rows_ * cols_];
    for (int i = 0; i < rows_ * cols_; ++i)
      data_[i] = other.data_[i];
  }
  return *this;
}

/**
 * @brief Swaps two rows of the matrix in place.
 */
void Matrix::swap_rows(int r1, int r2) {
  for (int j = 0; j < cols_; ++j)
    std::swap(data_[r1 * cols_ + j], data_[r2 * cols_ + j]);
}

/**
 * @brief Scales a row of the matrix by a given scalar.
 */
void Matrix::scale_row(int r, double scalar) {
  for (int j = 0; j < cols_; ++j)
    data_[r * cols_ + j] *= scalar;
}

/**
 * @brief Adds a scaled version of one row to another row.
 */
void Matrix::add_scaled_row(int src_row, int dest_row, double scalar) {
  for (int j = 0; j < cols_; ++j)
    data_[dest_row * cols_ + j] += data_[src_row * cols_ + j] * scalar;
}

/**
 * @brief Transposes the matrix and returns the result as a new Matrix.
 */
Matrix Matrix::transpose() const {
  Matrix result(cols_, rows_);
  for (int i = 0; i < rows_; ++i)
    for (int j = 0; j < cols_; ++j)
      result.data_[j * rows_ + i] = data_[i * cols_ + j];
  return result;
}

/**
 * @brief Returns the negative of the matrix.
 */
Matrix Matrix::operator-() const {
  Matrix result(rows_, cols_);
  for (int i = 0; i < rows_ * cols_; ++i) {
    result.data_[i] = -data_[i];
  }
  return result;
}

/**
 * @brief Compound addition assignment operator.
 */
Matrix &Matrix::operator+=(const Matrix &other) {
  if (rows_ != other.rows_ || cols_ != other.cols_)
    throw std::invalid_argument("Size mismatch for addition.");
  for (int i = 0; i < rows_ * cols_; ++i)
    data_[i] += other.data_[i];
  return *this;
}

/**
 * @brief Compound subtraction assignment operator.
 */
Matrix &Matrix::operator-=(const Matrix &other) {
  if (rows_ != other.rows_ || cols_ != other.cols_)
    throw std::invalid_argument("Size mismatch for subtraction.");
  for (int i = 0; i < rows_ * cols_; ++i)
    data_[i] -= other.data_[i];
  return *this;
}

/**
 * @brief Compound multiplication assignment operator.
 */
Matrix &Matrix::operator*=(double s) {
  for (int i = 0; i < rows_ * cols_; ++i)
    data_[i] *= s;
  return *this;
}

/**
 * @brief Compound division assignment operator.
 */
Matrix &Matrix::operator/=(double s) {
  return (*this) *= (1.0 / s);
}

// Binary operators

Matrix operator+(const Matrix &lhs, const Matrix &rhs) {
  Matrix result = lhs;
  result += rhs;
  return result;
}

Matrix operator-(const Matrix &lhs, const Matrix &rhs) {
  Matrix result = lhs;
  result -= rhs;
  return result;
}

Matrix operator*(const Matrix &m, double s) {
  Matrix result = m;
  result *= s;
  return result;
}

Matrix operator*(double s, const Matrix &m) {
  return m * s;
}

Matrix operator/(const Matrix &m, double s) {
  Matrix result = m;
  result /= s;
  return result;
}

/**
 * @brief Matrix-vector multiplication.
 */
Vector operator*(const Matrix &m, const Vector &v) {
  if (m.cols() != v.size())
    throw std::invalid_argument("Dimension mismatch for matrix-vector multiplication.");
  Vector result(m.rows());
  for (int i = 0; i < m.rows(); ++i) {
    double sum = 0;
    for (int j = 0; j < m.cols(); ++j)
      sum += m[i, j] * v[j];
    result[i] = sum;
  }
  return result;
}

/**
 * @brief Matrix-matrix multiplication.
 */
Matrix operator*(const Matrix &A, const Matrix &B) {
  if (A.cols() != B.rows())
    throw std::invalid_argument("Dimension mismatch for matrix-matrix multiplication.");
  Matrix result(A.rows(), B.cols());
  for (int i = 0; i < A.rows(); ++i)
    for (int j = 0; j < B.cols(); ++j) {
      double sum = 0;
      for (int k = 0; k < A.cols(); ++k)
        sum += A[i, k] * B[k, j];
      result[i, j] = sum;
    }
  return result;
}

/**
 * @brief Stream insertion operator.
 */
std::ostream &operator<<(std::ostream &os, const Matrix &m) {
  os << "[";
  for (int i = 0; i < m.rows(); ++i) {
    if (i > 0) os << ",\n ";
    os << "[";
    for (int j = 0; j < m.cols(); ++j) {
      if (j > 0) os << ", ";
      os << m[i, j];
    }
    os << "]";
  }
  os << "]";
  return os;
}

}  // namespace linalg
