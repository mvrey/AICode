//------------------------------------------------------------------------------
// File: SystemManager.h
// Purpose: Coordinates update order for ECS systems and legacy systems
//------------------------------------------------------------------------------
#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <vector>
#include <functional>

struct GameContext;

/// Manages system registration and execution order
class SystemManager {
public:
	SystemManager();
	~SystemManager();
	
	/// Update all registered systems in order
	/// @param delta_time Time step for this frame
	/// @param context Game context for systems that need it
	void Update(double delta_time, const GameContext* context);
	
	/// Register a system update function
	/// @param priority Higher priority = updated first
	/// @param update_func Function to call for updates
	void RegisterSystem(int priority, std::function<void(double, const GameContext*)> update_func);
	
	/// Clear all registered systems
	void Clear();

private:
	struct SystemEntry {
		int priority;
		std::function<void(double, const GameContext*)> update_func;
		
		bool operator<(const SystemEntry& other) const {
			return priority > other.priority; // Higher priority first
		}
	};
	
	std::vector<SystemEntry> systems_;
};

#endif // SYSTEM_MANAGER_H

