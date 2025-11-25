#include "AncientMythosTests.h"

#include <cstdio>

#include "RegistryTests.h"
#include "PrisonerEcsTests.h"

int AncientMythosTests() {
    RegistryTests();
    PrisonerEcsTests();

    std::puts("All ECS registry tests passed.");
    return 0;
}