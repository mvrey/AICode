#pragma once

#include <cstdio>
#include <exception>
#include <stdexcept>

#define TEST_CHECK(condition, message)                                      \
    do {                                                                    \
        if (!(condition)) {                                                 \
            throw std::runtime_error(message);                              \
        }                                                                   \
    } while (false)

inline bool RunNamedTest(const char* name, void (*test_fn)()) {
    try {
        test_fn();
        std::printf("[PASS] %s\n", name);
        return true;
    } catch (const std::exception& ex) {
        std::printf("[FAIL] %s: %s\n", name, ex.what());
        return false;
    } catch (...) {
        std::printf("[FAIL] %s: unknown exception\n", name);
        return false;
    }
}

