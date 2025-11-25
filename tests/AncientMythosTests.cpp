#include "src/RegistryTests.cpp"
#include "src/PrisonerEcsTests.cpp"

int main() {
	RegistryTests();
	PrisonerEcsTests();

	std::puts("All ECS registry tests passed.");
	return 0;
}