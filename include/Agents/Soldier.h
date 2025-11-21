#ifndef SOLDIER_H
#define SOLDIER_H

#include <cstdlib>
#include "Agent.h"
#include "../Pathfinding/astar.h"

class Soldier;

//Physic attributes
class SoldierBody : public AgentBody {
public:
	void update(double accumTime) override;

	int color_[4] = { 0, 0, 0, 200 };
	int size_ = 30;
};


//Psychic attributes. Managers are mind-pure.
class SoldierMind : public AgentMind {

friend class Soldier;

public:
	virtual void sense() override;
	virtual void reason() override;
	void update(double accumTime) override;

	SoldierStatus status_;
	float time_end_status_;
	Soldier* target_;
	Soldier* owner_;

	float detection_radius_ = 50;
};


class Soldier : public Agent {
public:

	Soldier();
	Soldier(const Soldier& orig);
	virtual ~Soldier();

	void init();
	void update(double accumTime) override;
	void render() override;

	virtual AgentBody* getBody() { return reinterpret_cast<SoldierBody*>(body_); };
	virtual ::MOMOS::SpriteHandle getImg() override { return img_; };

	SoldierMind* mind_;
	SoldierBody* body_;

	::MOMOS::SpriteHandle img_;

	// Current agent being hunted, if any
	::MOMOS::Vec2 target_;

	int current_target_door_ = 1;
	//The soldier is moving to the scape base
	bool escape_route_set_ = false;
	//The soldier has escaped
	bool escaped = false;
private:

	
};

#endif /* SOLDIER_H */