//------------------------------------------------------------------------------
// File: LegacyAgentManager.h
// Purpose: Wraps legacy Agent system to isolate it from game loop
//------------------------------------------------------------------------------
#ifndef LEGACY_AGENT_MANAGER_H
#define LEGACY_AGENT_MANAGER_H

class GameTimeService;

/// Manages legacy Agent system, isolating it from the main game loop
/// This allows the legacy system to be gradually migrated to ECS
class LegacyAgentManager {
public:
	LegacyAgentManager();
	~LegacyAgentManager();
	
	/// Update all legacy agents
	/// @param delta_time Time step for this frame
	/// @param time_service Service for getting game time
	void Update(double delta_time, GameTimeService* time_service);
	
	/// Render all legacy agents
	void Render();
	
	/// Check if any agents exist
	bool HasAgents() const;

private:
	// Forward declaration to avoid including Agent.h
	class AgentImpl;
};

#endif // LEGACY_AGENT_MANAGER_H

