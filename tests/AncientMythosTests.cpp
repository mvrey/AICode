#include "AncientMythosTests.h"

#include <cstdio>
#include <exception>

#include "RegistryTests.h"
#include "PrisonerEcsTests.h"

// Runs every discrete test suite so CI can invoke a single entry point.
int AncientMythosTests() {
    try {
        RegistryTests();
        PrisonerEcsTests();
    } catch (const std::exception& ex) {
        std::printf("[FATAL] Test harness exception: %s\n", ex.what());
        return 1;
    } catch (...) {
        std::puts("[FATAL] Test harness encountered an unknown exception.");
        return 1;
    }

    std::puts("All ECS registry tests completed.");
    return 0;
}