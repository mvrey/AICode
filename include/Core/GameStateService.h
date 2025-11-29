//------------------------------------------------------------------------------
// File: GameStateService.h
// Purpose: Tracks game initialization and state
//------------------------------------------------------------------------------
#ifndef GAME_STATE_SERVICE_H
#define GAME_STATE_SERVICE_H

/// Service that tracks game initialization and state
class GameStateService {
public:
	GameStateService();
	
	/// Check if pawns have been created
	bool ArePawnsCreated() const { return pawns_created_; }
	
	/// Mark pawns as created
	void SetPawnsCreated(bool created) { pawns_created_ = created; }

private:
	bool pawns_created_;
};

#endif // GAME_STATE_SERVICE_H

