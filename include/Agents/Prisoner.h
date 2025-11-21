#ifndef PRISONER_H
#define PRISONER_H

#include "Agent.h"
#include "../Pathfinding/astar.h"
#include "../ecs/PrisonerEcs.h"
#include "../ecs/components/PrisonerComponents.h"
#include <cstdlib>

class Prisoner;

//Physic attributes
class PrisonerBody : public AgentBody {
public:
	void update(double accumTime) override;

	int color_[4] = { 0, 0, 0, 200 };
	int size_ = 30;
};


//Psychic attributes. Managers are mind-pure.
class PrisonerMind : public AgentMind {

friend class Prisoner;

public:
	virtual void sense() override;
	virtual void reason() override;
	void update(double accumTime) override;

	bool isAlive();
	bool isActive();

	PrisonerStatus status_;
	double time_end_status_;
	Prisoner* target_;
	Prisoner* owner_;
};


class Prisoner : public Agent {
public:

	Prisoner();
	Prisoner(const Prisoner& orig);
	virtual ~Prisoner();

	void init();
	void update(double accumTime) override;
	void render() override;

	virtual AgentBody* getBody() { return reinterpret_cast<PrisonerBody*>(body_); };
	virtual MOMOS::SpriteHandle getImg() override { return img_; };

	short GetWorkingShift() const;
	void SetWorkingShift(short shift);

	ECS::Entity GetEntity() const { return ecs_entity_; }

	PrisonerMind* mind_;
	PrisonerBody* body_;

	MOMOS::SpriteHandle img_;

	ECS::PrisonerStateComponent& GetStateComponent();


private:
	const ECS::PrisonerStateComponent& GetStateComponent() const;
	ECS::MovementComponent& GetMovementComponent();
	ECS::TransformComponent& GetTransformComponent();
	ECS::SpriteComponent& GetSpriteComponent();

	void InitializeEcsComponents();
	void SyncEcsComponentsFromLegacy();

	ECS::Entity ecs_entity_;

	
};

#endif /* PRISONER_H */