#include "../../include/ecs/PawnSelection.h"
#include "../../include/ecs/PawnEcs.h"
#include "../../include/Camera.h"
#include "../../include/UI/InfoPanel.h"
#include "../../include/Map/MapResource.h"
#include <MOMOS/input.h>
#include <MOMOS/draw.h>
#include "../../include/ecs/components/PawnStateComponent.h"
#include "../../include/ecs/components/TransformComponent.h"
#include "../../include/ecs/Registry.h"
#include <vector>
#include <algorithm>

namespace PawnSelection {

static ECS::Entity g_selected_pawn;
static bool g_prev_key_was_pressed = false;
static bool g_next_key_was_pressed = false;

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
		// If camera was following a different pawn, stop it
		if (Camera::IsFollowing() && g_selected_pawn != Camera::GetFollowingEntity()) {
			Camera::StopFollowing();
		}
		InfoPanel::Get().SetMessage(closest_name);
		InfoPanel::Get().SetSelectedPawn(g_selected_pawn);
		// Clear cell resources when a pawn is selected
		InfoPanel::Get().SetSelectedCellResources(std::vector<MapResource>());
		return true;
	}
	
	// Clear selection if no pawn was found
	g_selected_pawn = ECS::Entity();
	InfoPanel::Get().SetSelectedPawn(ECS::Entity()); // Clear pawn selection in InfoPanel
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

bool HandleKeyboardNavigation() {
	// Check if ',' (previous) or '.' (next) is pressed
	bool prev_pressed = MOMOS::IsKeyPressed(',');
	bool next_pressed = MOMOS::IsKeyPressed('.');
	
	// Only trigger on key press (not while held down)
	bool prev_just_pressed = prev_pressed && !g_prev_key_was_pressed;
	bool next_just_pressed = next_pressed && !g_next_key_was_pressed;
	
	g_prev_key_was_pressed = prev_pressed;
	g_next_key_was_pressed = next_pressed;
	
	if (!prev_just_pressed && !next_just_pressed) {
		return false;
	}

	auto& registry = PawnECS::GetRegistry();
	
	// Collect all pawns with PawnStateComponent
	std::vector<ECS::Entity> pawns;
	registry.ForEach<ECS::PawnStateComponent>([&](ECS::Entity entity, ECS::PawnStateComponent& /*state*/) {
		pawns.push_back(entity);
	});
	
	if (pawns.empty()) {
		return false;
	}
	
	// Find current selected pawn index
	int current_index = -1;
	if (g_selected_pawn.IsValid()) {
		for (size_t i = 0; i < pawns.size(); ++i) {
			if (pawns[i] == g_selected_pawn) {
				current_index = static_cast<int>(i);
				break;
			}
		}
	}
	
	// If no pawn is selected, start from the first one
	if (current_index == -1) {
		current_index = 0;
	} else {
		// Navigate to previous or next
		if (prev_just_pressed) {
			current_index--;
			if (current_index < 0) {
				current_index = static_cast<int>(pawns.size()) - 1; // Wrap to end
			}
		} else if (next_just_pressed) {
			current_index++;
			if (current_index >= static_cast<int>(pawns.size())) {
				current_index = 0; // Wrap to beginning
			}
		}
	}
	
	// Select the pawn at the new index
	g_selected_pawn = pawns[current_index];
	
	// Update InfoPanel and focus camera on the selected pawn
	if (registry.HasComponent<ECS::PawnStateComponent>(g_selected_pawn)) {
		const auto& state = registry.GetComponent<ECS::PawnStateComponent>(g_selected_pawn);
		std::string pawn_name = state.name.empty() ? "Unnamed Pawn" : state.name;
		InfoPanel::Get().SetMessage(pawn_name);
		InfoPanel::Get().SetSelectedPawn(g_selected_pawn);
		InfoPanel::Get().SetSelectedCellResources(std::vector<MapResource>());
		
		// Focus camera on the selected pawn
		// If camera was following, stop it (user manually selected a different pawn)
		if (Camera::IsFollowing()) {
			Camera::StopFollowing();
		}
		if (registry.HasComponent<ECS::TransformComponent>(g_selected_pawn)) {
			const auto& transform = registry.GetComponent<ECS::TransformComponent>(g_selected_pawn);
			Camera::FocusOn(transform.position);
		}
	}
	
	return true;
}

} // namespace PawnSelection

