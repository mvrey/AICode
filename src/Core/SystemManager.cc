//------------------------------------------------------------------------------
// File: SystemManager.cc
// Purpose: Implementation of SystemManager
//------------------------------------------------------------------------------
#include "../../include/Core/SystemManager.h"
#include "../../include/Core/GameContext.h"
#include <algorithm>

SystemManager::SystemManager() {
}

SystemManager::~SystemManager() {
	Clear();
}

void SystemManager::Update(double delta_time, const GameContext* context) {
	// Sort by priority (highest first)
	std::sort(systems_.begin(), systems_.end());
	
	// Execute all systems
	for (auto& entry : systems_) {
		entry.update_func(delta_time, context);
	}
}

void SystemManager::RegisterSystem(int priority, std::function<void(double, const GameContext*)> update_func) {
	SystemEntry entry;
	entry.priority = priority;
	entry.update_func = update_func;
	systems_.push_back(entry);
}

void SystemManager::Clear() {
	systems_.clear();
}

