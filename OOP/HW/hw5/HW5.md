Objective
Welcome to this assignment! You will be working with a custom C++ library that implements fundamental linear algebra components. This module is designed to build, manipulate, and solve systems of linear equations using custom Vector and Matrix classes. This assignment will test your understanding of:

Memory Management: Managing dynamic arrays using raw pointers and strictly adhering to the Rule of Three to prevent memory leaks and dangling pointers.
Operator Overloading: Creating an intuitive, math-like syntax for C++ objects using member functions and non-member functions.
Object-Oriented Programming: Utilizing polymorphism, virtual functions, and inheritance to build an extensible framework for linear solvers.
The Skeleton Code
Your task is to implement the core mathematical and memory management logic of this framework. You will be provided with a skeleton of the library split into modular files (linear_solver.h, matrix.h/.cpp, vector.h/.cpp, and gaussian_elimination.h/.cpp), where several key implementations have been removed and marked with [todo] for you to complete.

Before writing any code, review the core components of the linalg namespace:

Vector: A dynamically sized 1D array of doubles. It supports basic arithmetic, dot products, and vector norms.
Matrix: A 2D mathematical object backed by a flat 1D dynamic array. It handles matrix arithmetic, transposition, and row operations.
LinearSolver: An abstract base class (interface) representing an algorithm that can solve the mathematical equation Ax=b.
GaussianElimination: A concrete implementation of LinearSolver that uses row operations to solve the system.
The specific tasks include:

In linear_solver.h:
Implement the virtual destructor to ensure proper cleanup for derived classes.
Declare the pure virtual solve() method.
In vector.h & matrix.h:
Implement the operator[] overloads for both const and non-const instances. For Matrix, remember to correctly map the 2D row/column coordinates to the flat 1D array.
In vector.cpp & matrix.cpp (Memory Management):
Implement Constructors to allocate memory and initialize values.
Implement the Rule of Three: The Destructor, Copy Constructor, and Copy Assignment operator (operator=). Ensure you perform deep copies of the data_ arrays to avoid double-free errors.
In vector.cpp & matrix.cpp (Math & Logic):
Complete the missing unary, binary, and compound arithmetic operators (-, -=, *, *=, /).
Implement specific operations like the dot() product in Vector and transpose(), swap_rows(), scale_row(), and add_scaled_row() in Matrix.
Implement the critical matrix-vector (A * x) and matrix-matrix (A * B) multiplication operators. Ensure you validate dimensions and throw std::invalid_argument upon mismatch.
In gaussian_elimination.cpp:
Implement the missing sections inside the solve(Matrix A, Vector b) method. You must complete the logic for partial pivoting (finding the pivot and swapping), forward elimination (zeroing out below the pivot), and back substitution to return the solution vector x.
Test Sample
The main.cpp includes test cases like this to test your implementation:

▾
nano_test::Register TestVectorDotProduct("Vector_Dot_Product", []() {
  Vector v1 = {1.0, 3.0, -5.0};
  Vector v2 = {4.0, -2.0, -1.0};
  
  // Dot product: (1*4) + (3*-2) + (-5*-1) = 4 - 6 + 5 = 3
  double dot = v1.dot(v2);
  nano_test::expect_approx_eq(dot, 3.0);
});
Optional Tasks (not included in the score)
Currently, our library is hardcoded to use double precision floats. In standard C++ libraries (like std::vector or Eigen), classes use Templates so they can store any data type. Upgrade your Vector and Matrix classes to be class templates (e.g., template <typename T> class Matrix). You will need to move your .cpp implementations into the header files (or an .inl file) since template code must be visible to the compiler at instantiation time. Once completed, demonstrate instantiating a Matrix<float> and a Matrix<int>.

Design Discussion
For those interested in software architecture and the darker corners of C++, consider these facts about our linear algebra framework's design:

1. Flat 1D Array for 2D Matrix Data
Look closely at the Matrix class. Even though a matrix is fundamentally 2D, we allocate its memory using a single, flat 1D array: data_(new double[rows * cols]). We calculate index offsets mathematically using r * cols_ + c. Why is this approach vastly superior for performance compared to using an array of pointers to arrays (e.g., double** data) or std::vector<std::vector<double>>? Consider the concepts of CPU Cache Locality and memory fragmentation.

2. Abstract Interfaces in Mathematical Libraries
We defined LinearSolver as a pure abstract interface and derived GaussianElimination from it. In standard scientific computing code, why is this useful? If a user later wanted to solve systems using another method (like Conjugate Gradient), how does this design allow them to do so without modifying the underlying systems that rely on a solver?

3. Pass-by-Value in the Solver Interface
Notice the signature Vector solve(Matrix A, Vector b). We are passing A and b by value, meaning a full copy of the matrix and vector is made automatically upon entering the function. Usually, in C++, we pass large objects by const reference (e.g., const Matrix& A) to avoid the performance cost of a copy. Why did we explicitly choose to pass by value here? Hint: Look at what the Gaussian Elimination algorithm does to the matrix A and vector b internally during the forward elimination step.

4. The Limits of a Unified Interface
We defined LinearSolver with a single interface. While this interface works perfectly for standard Gaussian Elimination or iterative methods, think about matrix factorization methods like LU Decomposition (A=LU) or Cholesky Decomposition. In real-world physics engines or machine learning, you often need to solve Ax=b for the same matrix A, but many different b vectors over time. Factorization allows you to perform the expensive decomposition step once, and save the result to quickly solve for new b vectors later. How does our current LinearSolver interface fail to capture this optimization? If you were to redesign the architecture to support separating the decomposition step from the solve step, what would it look like?

时间限制
10000 ms
内存限制
64 MB
CPU核数限制
1