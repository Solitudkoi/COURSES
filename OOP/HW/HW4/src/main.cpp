#include <iostream>
#include <cmath>
#include <numbers>
#include "ad_var.h"
#include "nano_test.h"

// 1) 1D Input -> 1D Output
nano_test::Register test_1d_to_1d("1D_to_1D", []() {
  ad::Graph g;
  double x_val = std::numbers::pi / 4.0; // 45 degrees
  ad::Var x = ad::Var::Input(g, x_val);
  
  ad::Var y = sin(x) * 2.0;
  g.forward();
  y.backward();

  double expected_y = std::sin(x_val) * 2.0;
  double expected_dy_dx = std::cos(x_val) * 2.0;

  nano_test::expect_approx_eq(y.value(), expected_y);
  nano_test::expect_approx_eq(x.adjoint(), expected_dy_dx);
});

// 2) nD Input -> 1D Output (n=2)
nano_test::Register test_nd_to_1d("nD_to_1D", []() {
  ad::Graph g;
  ad::Var x = ad::Var::Input(g, 2.0);
  ad::Var y = ad::Var::Input(g, 4.0);

  ad::Var f = pow2(x) + (x * y) + sqrt(y);
  g.forward();
  f.backward();

  double expected_f = 14.0; 
  double expected_df_dx = 8.0; 
  double expected_df_dy = 2.25;

  nano_test::expect_approx_eq(f.value(), expected_f);
  nano_test::expect_approx_eq(x.adjoint(), expected_df_dx);
  nano_test::expect_approx_eq(y.adjoint(), expected_df_dy);
});

// 3) nD Input -> nD Output (n=2)
nano_test::Register test_nd_to_nd("nD_to_nD", []() {
  ad::Graph g;
  double r_val = 5.0;
  double theta_val = std::numbers::pi / 6.0;

  ad::Var r = ad::Var::Input(g, r_val);
  ad::Var theta = ad::Var::Input(g, theta_val);

  ad::Var x_out = r * cos(theta);
  ad::Var y_out = r * sin(theta);
  
  g.forward();

  // -- First output variable (x_out) --
  x_out.backward(); 
  nano_test::expect_approx_eq(x_out.value(), r_val * std::cos(theta_val));
  nano_test::expect_approx_eq(r.adjoint(), std::cos(theta_val));
  nano_test::expect_approx_eq(theta.adjoint(), -r_val * std::sin(theta_val));

  // -- Second output variable (y_out) --
  y_out.backward();
  nano_test::expect_approx_eq(y_out.value(), r_val * std::sin(theta_val));
  nano_test::expect_approx_eq(r.adjoint(), std::sin(theta_val));
  nano_test::expect_approx_eq(theta.adjoint(), r_val * std::cos(theta_val));
});

// 4) Test Graph Reusability
nano_test::Register test_reusability("Graph_Reusability", []() {
  ad::Graph g;
  
  ad::Var x = ad::Var::Input(g, 2.0);
  ad::Var f = pow2(x) * 3.0;

  // Pass 1
  g.forward();
  f.backward();
  nano_test::expect_approx_eq(f.value(), 12.0);
  nano_test::expect_approx_eq(x.adjoint(), 12.0);

  // Pass 2 (Reuse)
  x.set_value(4.0);
  g.forward();
  f.backward();
  nano_test::expect_approx_eq(f.value(), 48.0);
  nano_test::expect_approx_eq(x.adjoint(), 24.0);
});

// 5) Practical Application: Iterative Optimization
nano_test::Register test_gradient_descent("Gradient_Descent", []() {
  ad::Graph g;
  
  double current_x = 0.0;
  double current_y = 0.0;
  
  ad::Var x = ad::Var::Input(g, current_x);
  ad::Var y = ad::Var::Input(g, current_y);

  ad::Var loss = pow2(x - 3.0) + pow2(y - 4.0);

  double learning_rate = 0.1;
  int iterations = 30;

  for (int i = 0; i < iterations; ++i) {
    g.forward();
    loss.backward();

    current_x -= learning_rate * x.adjoint();
    current_y -= learning_rate * y.adjoint();

    x.set_value(current_x);
    y.set_value(current_y);
  }

  // Verify convergence (loose tolerance because it's iterative)
  nano_test::expect_approx_eq(current_x, 3.0, 1e-2);
  nano_test::expect_approx_eq(current_y, 4.0, 1e-2);
});

// Run all registered tests
int main() {
  return nano_test::Registry::instance().run();
}
