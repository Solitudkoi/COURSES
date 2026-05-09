#ifndef NANO_TEST_H
#define NANO_TEST_H

#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(__has_include)
#if __has_include(<source_location>)
#include <source_location>
#define NANO_TEST_CURRENT_LOC SourceLocation::current()
#endif
#endif

namespace nano_test {

#ifdef NANO_TEST_CURRENT_LOC
using SourceLocation = std::source_location;
#endif

#ifndef NANO_TEST_CURRENT_LOC
struct SourceLocation {
  const char *file_name_ = "unknown";
  unsigned line_ = 0;

  const char *file_name() const noexcept { return file_name_; }
  unsigned line() const noexcept { return line_; }

  static SourceLocation current(const char *file = __builtin_FILE(),
                                unsigned line = __builtin_LINE()) noexcept {
    return {file, line};
  }
};
#define NANO_TEST_CURRENT_LOC SourceLocation::current()
#endif

struct TestFailure : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

template <typename T, typename U>
void expect_eq(const T &actual, const U &expected,
               SourceLocation loc = NANO_TEST_CURRENT_LOC) {
  if (actual != expected) {
    std::stringstream msg;
    msg << "Assertion failed at " << std::string(loc.file_name()) << ":"
        << std::to_string(loc.line()) << "\n"
        << "  Expected: " << expected << "\n"
        << "  Actual:   " << actual;
    throw TestFailure(msg.str());
  }
}

inline void expect_true(bool condition,
                        SourceLocation loc = NANO_TEST_CURRENT_LOC) {
  if (!condition) {
    std::stringstream msg;
    msg << "Condition false at " << std::string(loc.file_name()) << ":"
        << std::to_string(loc.line());
    throw TestFailure(msg.str());
  }
}

template <typename T, typename U>
void expect_approx_eq(const T &actual, const U &expected, double epsilon = 1e-6,
                      SourceLocation loc = NANO_TEST_CURRENT_LOC) {
  double diff = std::abs(static_cast<double>(actual) - static_cast<double>(expected));
  if (diff >= epsilon) {
    std::stringstream msg;
    msg << "Assertion failed at " << std::string(loc.file_name()) << ":"
        << std::to_string(loc.line()) << "\n"
        << "  Expected: ~" << expected << " (tolerance: " << epsilon << ")\n"
        << "  Actual:   " << actual << "\n"
        << "  Diff:     " << diff;
    throw TestFailure(msg.str());
  }
}

class Registry {
public:
  using TestFunc = std::function<void()>;

  struct TestCase {
    std::string name;
    TestFunc func;
  };

  static Registry &instance() {
    static Registry registry_instance;
    return registry_instance;
  }

  void add(const std::string &name, const TestFunc &func) {
    tests.push_back({name, func});
  }

  int run() {
    int passed = 0;
    int failed = 0;

    for (const auto &test : tests) {
      std::cout << "[ RUN      ] " << test.name << std::endl;
      try {
        test.func();
        std::cout << "[       OK ] " << test.name << std::endl;
        passed++;
      } catch (const std::exception &e) {
        std::cout << "[  FAILED  ] " << test.name << " - " << e.what() << std::endl;
        failed++;
      }
    }

    std::cout << "\n---------- Test Summary ----------\n";
    std::cout << "Total: " << passed + failed << ", Passed: " << passed
              << ", Failed: " << failed << "\n";
    std::cout << "----------------------------------\n";

    return failed == 0 ? 0 : 1;
  }

private:
  std::vector<TestCase> tests;
};

struct Register {
  Register(const std::string &name, const Registry::TestFunc &func) {
    Registry::instance().add(name, func);
  }
};

template <typename FixtureT>
struct RegisterFixture {
  RegisterFixture(const std::string &name, const std::function<void(FixtureT &)> &test_body) {
    Registry::instance().add(name, [test_body]() {
      FixtureT fixture;
      test_body(fixture);
    });
  }
};

} // namespace nano_test

#undef NANO_TEST_CURRENT_LOC

#endif // NANO_TEST_H
