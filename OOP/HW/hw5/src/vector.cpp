#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "vector.h"

namespace linalg {

/**
 * @brief Constructs a Vector of the given size, initializing all elements to 'val'.
 */
Vector::Vector(int size, double val) : size_(size), data_(new double[size]) {
  for (int i = 0; i < size_; ++i)
    data_[i] = val;
}

/**
 * @brief Constructs a Vector from an initializer list of doubles.
 */
Vector::Vector(std::initializer_list<double> init)
    : size_(static_cast<int>(init.size())), data_(new double[init.size()]) {
  int i = 0;
  for (double v : init)
    data_[i++] = v;
}

/**
 * @brief Destructor to clean up the dynamically allocated data array.
 */
Vector::~Vector() {
  delete[] data_;
}

/**
 * @brief Copy constructor for deep copying another Vector.
 */
Vector::Vector(const Vector &other)
    : size_(other.size_), data_(new double[other.size_]) {
  for (int i = 0; i < size_; ++i)
    data_[i] = other.data_[i];
}

/**
 * @brief Copy assignment operator for deep copying another Vector.
 */
Vector &Vector::operator=(const Vector &other) {
  if (this != &other) {
    delete[] data_;
    size_ = other.size_;
    data_ = new double[size_];
    for (int i = 0; i < size_; ++i)
      data_[i] = other.data_[i];
  }
  return *this;
}

/**
 * @brief Computes the Euclidean norm (L2 norm) of the vector.
 */
double Vector::euclidean_norm() const {
  double sum = 0;
  for (int i = 0; i < size_; ++i)
    sum += data_[i] * data_[i];
  return std::sqrt(sum);
}

/**
 * @brief Computes the infinity norm (L-infinity norm) of the vector.
 */
double Vector::infinity_norm() const {
  double max_abs = 0;
  for (int i = 0; i < size_; ++i)
    max_abs = std::max(max_abs, std::abs(data_[i]));
  return max_abs;
}

/**
 * @brief Computes the dot product of this vector with another vector.
 */
double Vector::dot(const Vector &other) const {
  if (size_ != other.size_)
    throw std::invalid_argument("Size mismatch for dot product.");
  double result = 0;
  for (int i = 0; i < size_; ++i)
    result += data_[i] * other.data_[i];
  return result;
}

/**
 * @brief Unary minus operator to negate all elements of the vector.
 */
Vector Vector::operator-() const {
  Vector result(size_);
  for (int i = 0; i < size_; ++i)
    result.data_[i] = -data_[i];
  return result;
}

/**
 * @brief Compound addition assignment operator.
 */
Vector &Vector::operator+=(const Vector &other) {
  if (size_ != other.size_)
    throw std::invalid_argument("Size mismatch for addition.");
  for (int i = 0; i < size_; ++i)
    data_[i] += other.data_[i];
  return *this;
}

/**
 * @brief Compound subtraction assignment operator.
 */
Vector &Vector::operator-=(const Vector &other) {
  if (size_ != other.size_)
    throw std::invalid_argument("Size mismatch for subtraction.");
  for (int i = 0; i < size_; ++i)
    data_[i] -= other.data_[i];
  return *this;
}

/**
 * @brief Compound multiplication assignment operator.
 */
Vector &Vector::operator*=(double s) {
  for (int i = 0; i < size_; ++i)
    data_[i] *= s;
  return *this;
}

/**
 * @brief Compound division assignment operator.
 */
Vector &Vector::operator/=(double s) {
  return (*this) *= (1.0 / s);
}

// Binary operators

Vector operator+(const Vector &lhs, const Vector &rhs) {
  Vector result = lhs;
  result += rhs;
  return result;
}

Vector operator-(const Vector &lhs, const Vector &rhs) {
  Vector result = lhs;
  result -= rhs;
  return result;
}

Vector operator*(const Vector &v, double s) {
  Vector result = v;
  result *= s;
  return result;
}

Vector operator*(double s, const Vector &v) {
  return v * s;
}

Vector operator/(const Vector &v, double s) {
  Vector result = v;
  result /= s;
  return result;
}

std::ostream &operator<<(std::ostream &os, const Vector &v) {
  os << "[";
  for (int i = 0; i < v.size(); ++i) {
    if (i > 0) os << ", ";
    os << v[i];
  }
  os << "]";
  return os;
}

}  // namespace linalg
