#include "nano_test.h"
#include <vector>

// ===============================================
// 1. Basic Stateless Tests
// ===============================================

// Use nano_test::Register for tests that don't need shared state.
nano_test::Register test1("Math.Addition", []() {
  nano_test::expect_eq(2 + 2, 4);
  nano_test::expect_true(1 + 4 > 3);
});

nano_test::Register test2("Math.IntentionalFailure", []() {
  // This will intentionally fail to demonstrate the framework's error output
  nano_test::expect_eq(10, 5);
});

// ===============================================
// 2. Fixture-Based Tests (RAII Setup/Teardown)
// ===============================================

// A fixture is a struct/class that defines a shared environment for tests.
// A brand new instance of this struct is created before EACH test runs, 
// ensuring tests do not corrupt each other's data.
struct VectorFixture {
  std::vector<int> data;

  // Setup: The constructor runs before the test body
  VectorFixture() {
    data.push_back(10);
    data.push_back(20);
  }

  // Teardown: The destructor runs automatically after the test body
  ~VectorFixture() {
    // Clean up any dynamic memory or open files here if necessary
    data.clear();
  }
};

// Use nano_test::RegisterFixture to pass the fixture into the lambda.
nano_test::RegisterFixture<VectorFixture> fixture_test_1(
  "VectorTest.InitialSize", 
  [](VectorFixture& fix) {
    // 'fix' contains the data prepared by the VectorFixture constructor
    nano_test::expect_eq(fix.data.size(), 2);
    nano_test::expect_eq(fix.data[0], 10);
  }
);

nano_test::RegisterFixture<VectorFixture> fixture_test_2(
  "VectorTest.PushBackIsolation", 
  [](VectorFixture& fix) {
    // Because a new fixture is created for this test, size is exactly 2 again.
    fix.data.push_back(30);
    nano_test::expect_eq(fix.data.size(), 3);
    nano_test::expect_eq(fix.data[2], 30);
  }
);

// ===============================================
// 3. Main Entry Point
// ===============================================

int main() {
  // The Registry Singleton has already collected all tests via static initialization.
  // Calling run() executes them all and prints the formatted summary.
  return nano_test::Registry::instance().run();
}