#ifndef LINALG_VECTOR_H
#define LINALG_VECTOR_H

#include <initializer_list>
#include <iostream>

namespace linalg {

/**
 * @brief A simple vector class for linear algebra operations.
 */
class Vector {
private:
  // Size of the vector and pointer to the data array.
  int size_;
  double *data_;

public:
  // Constructors
  Vector(int size, double val = 0.0);
  Vector(std::initializer_list<double> init);

  // Rule of Three
  ~Vector();
  Vector(const Vector &other);
  Vector &operator=(const Vector &other);

  // Accessors
  int size() const { return size_; }
  double &operator[](int i) { return data_[i]; }
  const double &operator[](int i) const { return data_[i]; }

  // Methods
  double euclidean_norm() const;
  double infinity_norm() const;
  double dot(const Vector &other) const;

  // Unary Operator
  Vector operator-() const;

  // Assignment Operators
  Vector &operator+=(const Vector &other);
  Vector &operator-=(const Vector &other);
  Vector &operator*=(double s);
  Vector &operator/=(double s);
};

// Symmetric Binary Operators
Vector operator+(const Vector &lhs, const Vector &rhs);
Vector operator-(const Vector &lhs, const Vector &rhs);
Vector operator*(const Vector &v, double s);
Vector operator*(double s, const Vector &v);
Vector operator/(const Vector &v, double s);
std::ostream &operator<<(std::ostream &os, const Vector &v);

}  // namespace linalg

#endif  // LINALG_VECTOR_H
