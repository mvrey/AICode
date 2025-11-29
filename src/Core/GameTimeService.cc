//------------------------------------------------------------------------------
// File: GameTimeService.cc
// Purpose: Implementation of GameTimeService
//------------------------------------------------------------------------------
#include "../../include/Core/GameTimeService.h"

GameTimeService::GameTimeService()
	: game_time_(0.0)
	, simulation_speed_(1.0f)
{
}

void GameTimeService::AdvanceTime(double delta) {
	game_time_ += GetEffectiveTimeStep(delta);
}

double GameTimeService::GetEffectiveTimeStep(double delta) const {
	return delta * static_cast<double>(simulation_speed_);
}

