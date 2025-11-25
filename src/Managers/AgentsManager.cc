/*
* File:   AgentsManager.cc
* Author: Marcos Vazquez
*
* Created on February 22, 2025, 00:33 AM
*/

#include "../../include/Managers/AgentsManager.h"

AgentsManager::AgentsManager() {}

AgentsManager::AgentsManager(const AgentsManager& orig) {}
AgentsManager::~AgentsManager() {}

std::vector<Prisoner*> AgentsManager::GetPrisoners() {
	return prisoners_;
}