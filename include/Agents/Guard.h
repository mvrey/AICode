#ifndef GUARD_H
#define GUARD_H

#include <cstdlib>
#include "Agent.h"
#include "../Pathfinding/astar.h"

class Guard;

//Physic attributes
class GuardBody : public AgentBody {
public:
	void update(double accumTime) override;

	int color_[4] = { 0, 0, 0, 200 };
};


//Psychic attributes. Managers are mind-pure.
class GuardMind : public AgentMind {

friend class Guard;

public:
	virtual void sense() override;
	virtual void reason() override;
	void update(double accumTime) override;

	bool isAlive();
	bool isActive();

	GuardStatus status_;
	double time_end_status_;

	Guard* owner_;

	//Radius of close interacting. Used to close doors.
	float detection_radius_ = 50;

	//Status-changing flags
	bool door_spotted_ = false;
	::MOMOS::Vec2 door_pos_;
	bool soldier_spotted_ = false;
};


class Guard : public Agent {
public:

	Guard();
	Guard(const Guard& orig);
	virtual ~Guard();

	void init();
	void update(double accumTime) override;
	void render() override;

	virtual AgentBody* getBody() { return reinterpret_cast<GuardBody*>(body_); };
	virtual ::MOMOS::SpriteHandle getImg() override { return img_; };

	GuardMind* mind_;
	GuardBody* body_;

	::MOMOS::SpriteHandle img_;
	
	///Returns vision orientation
	float getVisionSign(::MOMOS::Vec2 p1, ::MOMOS::Vec2 p2, ::MOMOS::Vec2 p3);
	///Checks if a given point is inside the vision cone
	bool isPointSighted(::MOMOS::Vec2 pt);
	///Calculates the array of points that form the vision cone
	void calculateVision();
	///Removes sections of the vision cone that collide with walls, effectively making walls stop vision
	void removeXRayPowers();
	///Draws the pathpoints of the vision cone
	void renderVision();

	//Vision cone lenght
	float vision_cone_length_ = 200.0f;
	//Vision cone width
	float vision_cone_amplitude_ = 80.0f;
	::MOMOS::Vec2 vision_cone_points_[300];
	float vision_cone_translated_points_[300 * 2];

private:

};

#endif /* GUARD_H */