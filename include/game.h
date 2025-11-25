#pragma once

// Runs per-frame input handling.
void Input();

// Updates legacy AI agents for the given time slice.
void UpdateAI(double accumTime);

// Renders the current frame.
void Draw();

// Ensures agents are spawned and returns whether the simulation started.
bool checkGameStarted();

// Advances the simulation by the provided timestep.
void Update(double timestep);

// Game entry point invoked from main.
int game(int argc, char** argv);

