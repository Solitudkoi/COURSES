#ifndef NANO_TEST_H
#define NANO_TEST_H

#include <cmath>
#include <functional>
#include <iostream>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace nano_test {

class TestFailure : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class Registry {
public:
  struct TestCase {
    std::string name;
    std::function<void()> func;
  };

  static Registry& instance() {
    static Registry reg;
    return reg;
  }

  void add(const std::string& name, std::function<void()> func) {
    tests_.push_back({name, std::move(func)});
  }

  int run() {
    int passed = 0, failed = 0;
    for (auto& tc : tests_) {
      try {
        tc.func();
        std::cout << "[PASS] " << tc.name << std::endl;
        ++passed;
      } catch (const TestFailure& e) {
        std::cout << "[FAIL] " << tc.name << "\n  " << e.what() << std::endl;
        ++failed;
      } catch (const std::exception& e) {
        std::cout << "[FAIL] " << tc.name << "\n  Unexpected exception: " << e.what() << std::endl;
        ++failed;
      }
    }
    std::cout << "\nResults: " << passed << " passed, " << failed << " failed, "
              << (passed + failed) << " total." << std::endl;
    return failed > 0 ? 1 : 0;
  }

private:
  std::vector<TestCase> tests_;
};

class Register {
public:
  Register(const std::string& name, std::function<void()> func) {
    Registry::instance().add(name, std::move(func));
  }
};

inline void expect_eq(int actual, int expected,
                      std::source_location loc = std::source_location::current()) {
  if (actual != expected) {
    std::stringstream msg;
    msg << "Assertion failed at "
        << loc.file_name() << ":" << loc.line() << "\n"
        << "  Expected: " << expected << ", Got: " << actual;
    throw TestFailure(msg.str());
  }
}

inline void expect_approx_eq(double actual, double expected, double tol = 1e-9,
                              std::source_location loc = std::source_location::current()) {
  if (std::abs(actual - expected) > tol) {
    std::stringstream msg;
    msg << "Assertion failed at "
        << loc.file_name() << ":" << loc.line() << "\n"
        << "  Expected: " << expected << ", Got: " << actual;
    throw TestFailure(msg.str());
  }
}

template <typename ExceptionT, typename Callable>
void expect_throw(Callable&& func,
                  std::source_location loc = std::source_location::current()) {
  try {
    func();
  } catch (const ExceptionT&) {
    return;
  } catch (...) {
    std::stringstream msg;
    msg << "Assertion failed at "
        << std::string(loc.file_name()) << ":"
        << std::to_string(loc.line()) << "\n"
        << "  Expected specific exception, but a different type was thrown.";
    throw TestFailure(msg.str());
  }

  std::stringstream msg;
  msg << "Assertion failed at "
      << std::string(loc.file_name()) << ":"
      << std::to_string(loc.line()) << "\n"
      << "  Expected exception, but nothing was thrown.";
  throw TestFailure(msg.str());
}

}  // namespace nano_test

#endif  // NANO_TEST_H
