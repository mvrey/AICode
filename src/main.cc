#include "../include/game.h"
#include "../tests/AncientMythosTests.h"

/// Entry point
int main(int argc, char **argv) {
	
	bool const TEST_MODE = true;

	if (TEST_MODE) {
		return AncientMythosTests();
	}
	else {
		return game(argc, argv);
	}
}