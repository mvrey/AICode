//------------------------------------------------------------------------------
// File: GameTimeService.h
// Purpose: Manages game time and simulation speed
//------------------------------------------------------------------------------
#ifndef GAME_TIME_SERVICE_H
#define GAME_TIME_SERVICE_H

/// Service that manages game time and simulation speed
class GameTimeService {
public:
	GameTimeService();
	
	/// Get current game time
	double GetGameTime() const { return game_time_; }
	
	/// Set game time
	void SetGameTime(double time) { game_time_ = time; }
	
	/// Advance game time by delta (adjusted by simulation speed)
	void AdvanceTime(double delta);
	
	/// Get current simulation speed multiplier
	float GetSimulationSpeed() const { return simulation_speed_; }
	
	/// Set simulation speed multiplier
	void SetSimulationSpeed(float speed) { simulation_speed_ = speed; }
	
	/// Get effective time step (delta * simulation_speed)
	double GetEffectiveTimeStep(double delta) const;

private:
	double game_time_;
	float simulation_speed_;
};

#endif // GAME_TIME_SERVICE_H

