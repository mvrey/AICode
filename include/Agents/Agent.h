/*
* File:   Agent.h
* Author: Marcos Vazquez
*
* Created on November 29, 2016, 6:35 PM
*
* A generic root Agent template
*/

#ifndef AGENT_H
#define AGENT_H


#include <ctgmath>
#include <vector>
#include <cstdlib>

#include "../config.h"
#include "../GameStatus.h"
#include "../Pathfinding/astar.h"


//Physic attributes
class AgentBody {
  public:
    AgentBody() {};
    AgentBody(const AgentBody& orig) {};
    virtual ~AgentBody() {};
    
	virtual void act();
    virtual void update(double accumTime);
	void stop();

	void setDirection(::MOMOS::Vec2 pos);
	void move(::MOMOS::Vec2 dir, double accumTime);
	float getDistanceTo(::MOMOS::Vec2 dest);

	Agent* owner_;

	::MOMOS::Vec2 pos_;
	::MOMOS::Vec2 direction_;
    int color_[4] = { 0, 0, 0, 200 };
    int size_ = 30;
	bool path_set_ = false;
};


//Psychic attributes. Managers are mind-pure.
class AgentMind {
  public:
    AgentMind() {};
    AgentMind(const AgentMind& orig) {};
    virtual ~AgentMind() {};
    
	virtual void sendMessage(Message m);
	virtual void receiveMessage(Message m);
	virtual void respondMessage(Message m, bool response);

	virtual void sense();
	virtual void reason();

    virtual void update(double accumTime);

	Agent* owner_;

	std::vector<Message> messages_;

	bool movement_finished_ = true;
};


//Fwd declaration
class PathCommand;

class Agent {
public:
  Agent();
  Agent(const Agent& orig);
  virtual ~Agent();
  
  virtual void update(double accumTime);
  virtual void render();

  static std::vector<Agent*> agents_; 
  static unsigned int last_id_;
  static int last_updated_id_;

  AliveStatus aliveStatus_;
  unsigned int id_;
  
  virtual AgentBody* getBody() { return nullptr; };
  virtual ::MOMOS::SpriteHandle getImg() { return img_; };

  /// Searches for the best path to the destination
  void setPathTo(::MOMOS::Vec2 dest);
  /// Searches for a path to a specific room
  bool goToRoom(Room room);
  /// Returns true if the current path has been completed
  bool moveFollowingPath();
  /// Clears all variables associated with movement along a path
  void clearMovement();

  AgentBody* body_ = nullptr;
  AgentMind* mind_ = nullptr;

  /// MOVEMENT METHODS & STUFF
  void moveDeterministic();
  void moveRandom();
  void moveTracking();
  void movePattern();

  double last_movement_update_;
  double movement_threshold_;

  std::vector<PatternStep> pattern_steps_;
  int step_num_;

  std::vector<::MOMOS::Vec2> deterministic_steps_;
  unsigned int deterministic_step_num_;

  float speed_;

  Path* movement_path_;

  // Current agent being hunted, if any
  Agent* target_;

  // Pathfinding command to be sent to the Pathfinder manager
  PathCommand* path_cmd_;

protected:
	::MOMOS::SpriteHandle img_;
  
private:
  
};

#endif /* AGENT_H */