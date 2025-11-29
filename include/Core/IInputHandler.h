//------------------------------------------------------------------------------
// File: IInputHandler.h
// Purpose: Interface for input handlers with priority-based processing
//------------------------------------------------------------------------------
#ifndef I_INPUT_HANDLER_H
#define I_INPUT_HANDLER_H

/// Interface for input handlers that process user input
/// Handlers are processed in priority order, and processing stops when a handler consumes input
class IInputHandler {
public:
	virtual ~IInputHandler() = default;
	
	/// Process input for this frame
	/// @return true if input was consumed (stops further processing), false otherwise
	virtual bool HandleInput() = 0;
	
	/// Get priority for processing order (higher = processed first)
	virtual int GetPriority() const = 0;
};

#endif // I_INPUT_HANDLER_H

