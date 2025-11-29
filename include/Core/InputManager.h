//------------------------------------------------------------------------------
// File: InputManager.h
// Purpose: Coordinates input handling across multiple handlers
//------------------------------------------------------------------------------
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "IInputHandler.h"
#include <vector>
#include <algorithm>

/// Manages input handlers and processes them in priority order
class InputManager {
public:
	InputManager();
	~InputManager();
	
	/// Register an input handler (takes ownership)
	void RegisterHandler(IInputHandler* handler);
	
	/// Process all input handlers in priority order
	/// Stops when a handler consumes input (returns true)
	void ProcessInput();
	
	/// Clear all registered handlers
	void Clear();

private:
	std::vector<IInputHandler*> handlers_;
	
	// Sort handlers by priority (highest first)
	void SortHandlers();
};

#endif // INPUT_MANAGER_H

