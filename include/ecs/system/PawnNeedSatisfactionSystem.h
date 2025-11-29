//------------------------------------------------------------------------------
// File: PawnNeedSatisfactionSystem.h
// Purpose: System that integrates PawnAI and PawnFSM for need satisfaction
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_NEED_SATISFACTION_SYSTEM_H
#define ECS_PAWN_NEED_SATISFACTION_SYSTEM_H

#include "../System.h"

namespace ECS {

/// System that handles pawn need satisfaction by integrating PawnAI and PawnFSM
class PawnNeedSatisfactionSystem : public IEcsSystem {
public:
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PAWN_NEED_SATISFACTION_SYSTEM_H

