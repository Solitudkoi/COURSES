#ifndef NANO_TEST_H
#define NANO_TEST_H

#include <functional>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

namespace nano_test {

// --- 1. Exception for Failures ---

/**
 * @brief Custom exception type for test failures, inheriting from std::runtime_error.
 * This allows us to throw detailed error messages when assertions fail.
 */
struct TestFailure : public std::runtime_error 
{
  using std::runtime_error::runtime_error;
};

// --- 2. Assertions ---

/**
 * @brief Asserts that 'actual' is equal to 'expected'. If not, throws a TestFailure with a detailed message.
 * @tparam T The type of the actual value.
 * @tparam U The type of the expected value.
 * @param actual The value produced by the test.
 * @param expected The value that the test expects.
 * @param loc Source location information (file and line) for error reporting, defaults to the call site.
 */

//模板断言函数。用于判断两个值是否相等，不相等时抛出包含详细信息的 `TestFailure`
template <typename T, typename U>
void expect_eq(const T &actual, const U &expected,
               std::source_location loc = std::source_location::current()) 
{
  // [todo] Implement this function.
  // 1. Compare actual and expected.
  // 2. If they are not equal, construct an error message using std::stringstream.
  // 3. Include the file name and line number using 'loc.file_name()' and 'loc.line()'.
  // 4. Display the expected and actual values in the error message.
  // 5. Throw a TestFailure exception containing the formatted string.
        
     if(actual != expected)
     {
          std::stringstream msg;
          msg << "Assertion failed at "
              << std::string(loc.file_name()) << ":"
              << std::to_string(loc.line()) << "\n"
              << "  Expected: " << expected << "\n"
              << "  Actual:   " << actual;
          throw TestFailure(msg.str());
     }

}

/**
 * @brief Asserts that 'condition' is true. If not, throws a TestFailure with a detailed message.
 * @param condition The boolean value to be checked.
 * @param loc Source location information (file and line) for error reporting, defaults to the call site.
 */
inline
void expect_true(bool condition,
                 std::source_location loc = std::source_location::current()) {
  if (!condition) {
    std::stringstream msg;
    msg << "Condition false at "
        << std::string(loc.file_name()) << ":"
        << std::to_string(loc.line());
    throw TestFailure(msg.str());
  }
}

// --- 3. The Registry (Singleton) ---

/**
 * @brief A singleton class that holds all registered tests and runs them.
 * Tests are registered via the Register and RegisterFixture helper structs.
 */
class Registry 
{
public:
  // Type alias for a test function, which is a void function with no parameters.
  using TestFunc = std::function<void()>;

  // Struct to hold test case information (name and function).
  struct TestCase {
    std::string name;
    TestFunc func;
  };

  /**
   * @brief Accessor for the singleton instance of the Registry.
   * Ensures that only one instance of Registry exists throughout the program.
   * @return Reference to the singleton Registry instance.
   */
  static Registry &instance() {
    // [todo] Implement the Singleton pattern here.
    // Hint: Use a static local variable.      
    // Remove the following lines once implemented.
    //std::cerr << "Registry::instance() not implemented yet.\n";
    //std::exit(-1);
    static Registry registry_instance;
    return registry_instance;
  }

  /**
   * @brief Adds a test case to the registry.
   * @param name The name of the test case.
   * @param func The function that implements the test case.
   */
  void add(const std::string &name, const TestFunc &func) {
    // [todo] Implement this function to add a test case to the registry.
    tests.push_back({name, func});
  }

  /**
   * @brief Runs all registered test cases and reports the results.
   * Catches exceptions thrown by test cases to determine pass/fail status.
   * @return 0 if all tests passed, 1 if any test failed.
   */
  int run() {
    // Counters for passed and failed tests.
    int passed = 0;
    int failed = 0;

    // [todo] Loop through all registered tests and execute them.
    for(const auto& test : tests)
    {
        std::cout << "[ RUN      ] " << test.name <<std::endl;
        try{
            test.func();
            std::cout << "[       OK ] "<< test.name <<std::endl;
            passed++;
        } catch (const std::exception& e)
        {
            std::cout << "[  FAILED  ] " << test.name << " - "<< e.what() << std::endl;
            failed++;
        }
    }
    // Final summary output
    std::cout << "\n---------- Test Summary ----------\n";
    std::cout << "Total: " << passed + failed
              << ", Passed: " << passed
              << ", Failed: " << failed << "\n";
    std::cout << "----------------------------------\n";

    return failed == 0 ? 0 : 1;
  }

private:
  // Vector to hold all registered test cases.
  std::vector<TestCase> tests;
};

// --- 4. Registration Helpers ---

/**
 * @brief Helper struct to register a simple test case without shared state.
 * The constructor takes the test name and the test function, and registers it with the Registry singleton.
 */
struct Register {
  Register(const std::string &name, const Registry::TestFunc &func) {
    Registry::instance().add(name, func);
  }
};

/**
 * @brief Helper struct to register a test case that uses a fixture for setup and teardown.
 * @tparam FixtureT The type of the fixture to be used for this test case.
 */
template <typename FixtureT>
struct RegisterFixture {
  /**
   * @brief Constructor that registers a fixture-based test case.
   * The test_body is a function that takes a reference to the fixture and contains the test logic.
   * A new instance of the fixture is created for each test run, ensuring isolation between tests.
   * @param name The name of the test case.
   * @param test_body The function that implements the test logic, taking a FixtureT reference as an argument.
   */
  RegisterFixture(const std::string &name, const std::function<void(FixtureT &)> &test_body) {
    Registry::instance().add(name, [test_body]() {
      FixtureT fixture;   // SETUP (Constructor)
      test_body(fixture); // RUN
    });                   // TEARDOWN (Destructor)
  }
};
} // namespace nano_test

#endif // NANO_TEST_H