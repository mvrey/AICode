/*
* File:   AgentsManager.h
* Author: Marcos Vazquez
*
* Created on Nov 22, 2025, 00:33 AM
*
*/

#ifndef AGENTSMANAGER_H
#define AGENTSMANAGER_H

#include "../config.h"

//forward declarations for agent cache
class Guard;
class Prisoner;
class Soldier;

class AgentsManager {
public:
	AgentsManager();
	AgentsManager(const AgentsManager& orig);
	virtual ~AgentsManager();
	
	bool g_agents_created = false;

	std::vector<Prisoner*> GetPrisoners();

private:

	std::vector<Prisoner*> prisoners_;
};

#endif /* AGENTSMANAGER_h */