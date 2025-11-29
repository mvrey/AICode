//------------------------------------------------------------------------------
// File: LegacyAgentManager.cc
// Purpose: Implementation of LegacyAgentManager
//------------------------------------------------------------------------------
#include "../../include/Core/LegacyAgentManager.h"
#include "../../include/Core/GameTimeService.h"
#include "../../include/Agents/Agent.h"
#include "../../include/config.h"

LegacyAgentManager::LegacyAgentManager() {
}

LegacyAgentManager::~LegacyAgentManager() {
}

void LegacyAgentManager::Update(double delta_time, GameTimeService* time_service) {
	if (!time_service) {
		return;
	}
	
	// Keep track of remaining time left on this frame
	double start, end;
	bool timeout = false;
	int j = (Agent::last_updated_id_ == -1) ? 0 : Agent::last_updated_id_;

	for (unsigned int i = j; i < Agent::agents_.size() && !timeout; i++) {
		Agent* agent = Agent::agents_[i];

		start = time_service->GetGameTime();
		agent->update(delta_time);
		end = time_service->GetGameTime();

		if (agent->getBody() && agent->getBody()->pos_.y > Screen::height) {
			agent->aliveStatus_ = kDead;
		}

		delta_time -= end - start;
		if (delta_time <= 0.0f) {
			Agent::last_updated_id_ = i;
			timeout = true;
		}

		// Once the last agent has been updated, return to the first one if there's still time
		if (i == j - 1) {
			i = 0;
		}
	}

	if (!timeout) {
		Agent::last_updated_id_ = -1;
	}
}

void LegacyAgentManager::Render() {
	// Draw agents
	for (unsigned int i = 0; i < Agent::agents_.size(); i++) {
		// Skip dead agents and managers (agents with no body)
		if (Agent::agents_[i]->aliveStatus_ == kAlive && Agent::agents_[i]->getBody()) {
			Agent::agents_[i]->render();
		}
	}
}

bool LegacyAgentManager::HasAgents() const {
	return !Agent::agents_.empty();
}

