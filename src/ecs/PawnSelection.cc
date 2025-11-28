#include "../../include/ecs/PawnSelection.h"
#include "../../include/ecs/PawnEcs.h"
#include "../../include/Camera.h"
#include "../../include/UI/InfoPanel.h"
#include <MOMOS/input.h>
#include <MOMOS/draw.h>
#include "../../include/ecs/components/PawnStateComponent.h"
#include "../../include/ecs/components/TransformComponent.h"

namespace PawnSelection {

static ECS::Entity g_selected_pawn;

bool HandleClick() {
	if (!MOMOS::MouseButtonDown(1)) {
		return false;
	}

	::MOMOS::Vec2 mouse_screen = {
		static_cast<float>(MOMOS::MousePositionX()),
		static_cast<float>(MOMOS::MousePositionY())
	};
	::MOMOS::Vec2 world_click = Camera::ScreenToWorld(mouse_screen);

	auto& registry = PawnECS::GetRegistry();
	const float click_radius = 32.0f;
	float best_distance_sq = click_radius * click_radius;
	bool found = false;
	std::string closest_name;
	struct PawnClickBreak {};

	try {
		registry.ForEach<ECS::PawnStateComponent>([&](ECS::Entity entity, ECS::PawnStateComponent& state) {
			if (!registry.HasComponent<ECS::TransformComponent>(entity)) {
				return;
			}
			auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);
			float dx = transform.position.x - world_click.x;
			float dy = transform.position.y - world_click.y;
			float distance_sq = dx * dx + dy * dy;
			if (distance_sq <= best_distance_sq) {
				best_distance_sq = distance_sq;
				found = true;
				closest_name = state.name.empty() ? "Unnamed Pawn" : state.name;
				g_selected_pawn = entity;
				throw PawnClickBreak();
			}
		});
	} catch (const PawnClickBreak&) {
	}

	if (found) {
		InfoPanel::Get().SetMessage(closest_name);
		return true;
	}
	
	// Clear selection if no pawn was found
	g_selected_pawn = ECS::Entity();
	return false;
}

void DrawSelection() {
	if (!g_selected_pawn.IsValid()) {
		return;
	}

	auto& registry = PawnECS::GetRegistry();
	if (!registry.HasComponent<ECS::TransformComponent>(g_selected_pawn)) {
		return;
	}

	auto& transform = registry.GetComponent<ECS::TransformComponent>(g_selected_pawn);
	const float half_screen = 16.0f;
	const float half_world = half_screen / Camera::Zoom();

	::MOMOS::Vec2 top_left = {
		transform.position.x - half_world,
		transform.position.y - half_world
	};
	::MOMOS::Vec2 bottom_right = {
		transform.position.x + half_world,
		transform.position.y + half_world
	};

	auto top_left_screen = Camera::WorldToScreen(top_left);
	auto bottom_right_screen = Camera::WorldToScreen(bottom_right);

	MOMOS::DrawSetStrokeColor(100, 255, 100, 255);
	MOMOS::DrawLine(top_left_screen.x, top_left_screen.y, bottom_right_screen.x, top_left_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, top_left_screen.y, bottom_right_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, bottom_right_screen.y, top_left_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(top_left_screen.x, bottom_right_screen.y, top_left_screen.x, top_left_screen.y);
}

ECS::Entity GetSelectedPawn() {
	return g_selected_pawn;
}

void ClearSelection() {
	g_selected_pawn = ECS::Entity();
}

} // namespace PawnSelection

