//------------------------------------------------------------------------------
// File: InputManager.cc
// Purpose: Implementation of InputManager
//------------------------------------------------------------------------------
#include "../../include/Core/InputManager.h"

InputManager::InputManager() {
}

InputManager::~InputManager() {
	Clear();
}

void InputManager::RegisterHandler(IInputHandler* handler) {
	if (handler) {
		handlers_.push_back(handler);
		SortHandlers();
	}
}

void InputManager::ProcessInput() {
	for (auto* handler : handlers_) {
		if (handler && handler->HandleInput()) {
			// Handler consumed input, stop processing
			return;
		}
	}
}

void InputManager::Clear() {
	for (auto* handler : handlers_) {
		delete handler;
	}
	handlers_.clear();
}

void InputManager::SortHandlers() {
	std::sort(handlers_.begin(), handlers_.end(),
		[](IInputHandler* a, IInputHandler* b) {
			return a->GetPriority() > b->GetPriority();
		});
}

