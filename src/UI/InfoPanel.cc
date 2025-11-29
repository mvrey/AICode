#include "../../include/UI/InfoPanel.h"
#include "../../include/ecs/Registry.h"
#include "../../include/ecs/Entity.h"
#include "../../include/ecs/components/NeedsControllerComponent.h"
#include "../../include/Needs/NeedId.h"
#include "../../include/Needs/INeed.h"
#include "../../include/ecs/components/PawnStateComponent.h"
#include "../../include/ecs/PawnEcs.h"
#include "../../include/Map/MapResource.h"
#include "../../include/Camera.h"
#include <utility>

#include <MOMOS/draw.h>
#include <algorithm>
#include <cstdio>

InfoPanel& InfoPanel::Get() {
	static InfoPanel instance;
	return instance;
}

InfoPanel::InfoPanel()
	: message_("Nothing to show yet"), selected_pawn_(), selected_cell_resources_() {
	follow_button_bounds_[0] = follow_button_bounds_[1] = follow_button_bounds_[2] = follow_button_bounds_[3] = 0.0f;
}

void InfoPanel::SetMessage(std::string text) {
	if (text.empty()) {
		message_ = "Nothing to show yet";
	} else {
		message_ = std::move(text);
	}
}

void InfoPanel::SetSelectedPawn(ECS::Entity pawn) {
	selected_pawn_ = pawn;
}

void InfoPanel::SetSelectedCellResources(const std::vector<MapResource>& resources) {
	selected_cell_resources_ = resources;
	// Debug: Print resource count (can be removed later)
	printf("InfoPanel: Set %zu resources\n", resources.size());
	for (size_t i = 0; i < resources.size(); ++i) {
		if (resources[i].type != nullptr) {
			printf("  Resource %zu: %s (amount: %d)\n", i, resources[i].type->name.c_str(), resources[i].amount);
		} else {
			printf("  Resource %zu: null type (amount: %d)\n", i, resources[i].amount);
		}
	}
}

void InfoPanel::Draw() const {
	// Determine panel height based on what we're showing
	bool showing_needs = selected_pawn_.IsValid();
	bool showing_resources = !selected_cell_resources_.empty();
	
	float panel_height = kPanelHeight;
	if (showing_needs) {
		panel_height = kPanelHeightWithNeeds;
	}
	if (showing_resources) {
		// If showing both needs and resources, use the larger height
		panel_height = std::max(panel_height, kPanelHeightWithResources);
		// If only showing resources, use resources height
		if (!showing_needs) {
			panel_height = kPanelHeightWithResources;
		}
	}
	
	float panel_width = static_cast<float>(Screen::width);
	float panel_top_y = static_cast<float>(Screen::height) - panel_height;
	
	float points[10] = {
		0.0f, panel_top_y,
		panel_width, panel_top_y,
		panel_width, panel_top_y + panel_height,
		0.0f, panel_top_y + panel_height,
		0.0f, panel_top_y
	};

	MOMOS::DrawSetFillColor(50, 50, 50, static_cast<unsigned char>(0.8f * 255));
	MOMOS::DrawSolidPath(points, 5);

	// Draw message text
	MOMOS::DrawSetFillColor(255, 255, 255, 255);
	MOMOS::DrawSetTextSize(kTextSize);
	MOMOS::DrawText(kTextPadding, panel_top_y + kTextPadding + 2.0f, message_.c_str());

	float current_y = panel_top_y + kTextSize + kTextPadding * 2.0f;

	// Draw needs bars if a pawn is selected
	if (showing_needs) {
		// Get registry from PawnECS
		auto& registry = PawnECS::GetRegistry();
		
		// Draw pawn state first
		if (registry.HasComponent<ECS::PawnStateComponent>(selected_pawn_)) {
			DrawPawnState(registry, selected_pawn_, current_y);
			current_y += kResourceLineSpacing; // Add spacing after state
		}
		
		// Draw follow button
		float button_x = panel_width - kFollowButtonWidth - kTextPadding;
		float button_y = panel_top_y + kTextPadding;
		DrawFollowButton(button_x, button_y);
		
		if (registry.HasComponent<ECS::NeedsControllerComponent>(selected_pawn_)) {
			DrawNeedsBars(registry, selected_pawn_, current_y);
			current_y += (kNeedBarHeight + kNeedBarSpacing) * 3.0f; // 3 need bars
		}
	}

	// Draw resources list if a cell with resources is selected
	if (showing_resources) {
		DrawResourcesList(current_y);
	}
}

void InfoPanel::DrawNeedsBars(const ECS::Registry& registry, ECS::Entity pawn, float start_y) const {
	const auto& needs_comp = registry.GetComponent<ECS::NeedsControllerComponent>(pawn);
	if (!needs_comp.controller) {
		return;
	}
	
	float x = kTextPadding;
	float y = start_y;
	
	// Get need values directly from NeedsController
	INeed* hunger_need = needs_comp.controller->GetNeed(NeedId::Hunger);
	INeed* energy_need = needs_comp.controller->GetNeed(NeedId::Energy);
	INeed* joy_need = needs_comp.controller->GetNeed(NeedId::Joy);
	
	// Draw each need bar
	float hunger_value = hunger_need ? hunger_need->GetValue() : 0.0f;
	float energy_value = energy_need ? energy_need->GetValue() : 0.0f;
	float joy_value = joy_need ? joy_need->GetValue() : 0.0f;
	
	DrawNeedBar("Hunger:", hunger_value, x, y, kNeedBarWidth, kNeedBarHeight);
	y += kNeedBarHeight + kNeedBarSpacing;
	
	DrawNeedBar("Energy:", energy_value, x, y, kNeedBarWidth, kNeedBarHeight);
	y += kNeedBarHeight + kNeedBarSpacing;
	
	DrawNeedBar("Joy:", joy_value, x, y, kNeedBarWidth, kNeedBarHeight);
}

void InfoPanel::DrawNeedBar(const char* name, float value, float x, float y, float width, float height) const {
	// Clamp value to [0, 1]
	float clamped_value = std::max(0.0f, std::min(1.0f, value));
	
	// Draw label
	MOMOS::DrawSetFillColor(255, 255, 255, 255);
	MOMOS::DrawSetTextSize(kTextSize - 2.0f);
	MOMOS::DrawText(x, y + height - 2.0f, name);
	
	// Calculate bar position (after label)
	float bar_x = x + kNeedLabelWidth;
	float bar_y = y;
	
	// Draw background (empty bar)
	MOMOS::DrawSetFillColor(40, 40, 40, 255);
	float bg_points[10] = {
		bar_x, bar_y,
		bar_x + width, bar_y,
		bar_x + width, bar_y + height,
		bar_x, bar_y + height,
		bar_x, bar_y
	};
	MOMOS::DrawSolidPath(bg_points, 5);
	
	// Draw filled portion based on value
	if (clamped_value > 0.0f) {
		// Color gradient: red (low) -> yellow (medium) -> green (high)
		unsigned char r, g, b;
		if (clamped_value < 0.5f) {
			// Red to yellow
			float t = clamped_value * 2.0f;
			r = 255;
			g = static_cast<unsigned char>(t * 255.0f);
			b = 0;
		} else {
			// Yellow to green
			float t = (clamped_value - 0.5f) * 2.0f;
			r = static_cast<unsigned char>((1.0f - t) * 255.0f);
			g = 255;
			b = 0;
		}
		
		MOMOS::DrawSetFillColor(r, g, b, 255);
		float filled_width = width * clamped_value;
		float filled_points[10] = {
			bar_x, bar_y,
			bar_x + filled_width, bar_y,
			bar_x + filled_width, bar_y + height,
			bar_x, bar_y + height,
			bar_x, bar_y
		};
		MOMOS::DrawSolidPath(filled_points, 5);
	}
	
	// Draw border
	MOMOS::DrawSetStrokeColor(200, 200, 200, 255);
	MOMOS::DrawLine(bar_x, bar_y, bar_x + width, bar_y);
	MOMOS::DrawLine(bar_x + width, bar_y, bar_x + width, bar_y + height);
	MOMOS::DrawLine(bar_x + width, bar_y + height, bar_x, bar_y + height);
	MOMOS::DrawLine(bar_x, bar_y + height, bar_x, bar_y);
}

void InfoPanel::DrawResourcesList(float start_y) const {
	if (selected_cell_resources_.empty()) {
		return;
	}

	MOMOS::DrawSetFillColor(255, 255, 255, 255);
	MOMOS::DrawSetTextSize(kTextSize - 2.0f);
	
	float x = kTextPadding;
	float y = start_y;
	
	// Draw header
	MOMOS::DrawSetTextSize(kTextSize);
	MOMOS::DrawText(x, y, "Resources:");
	y += kResourceLineSpacing;
	
	// Draw each resource
	MOMOS::DrawSetTextSize(kTextSize - 2.0f);
	int valid_resources = 0;
	for (const auto& resource : selected_cell_resources_) {
		if (resource.type != nullptr && resource.amount > 0) {
			char resource_text[128];
			snprintf(resource_text, sizeof(resource_text), "  - %s: %d", 
				resource.type->name.c_str(), resource.amount);
			MOMOS::DrawText(x, y, resource_text);
			y += kResourceLineSpacing;
			valid_resources++;
		}
	}
	
	// If no valid resources were found, show a message
	if (valid_resources == 0 && !selected_cell_resources_.empty()) {
		MOMOS::DrawText(x, y, "  (resources have invalid data)");
	}
}

void InfoPanel::DrawPawnState(const ECS::Registry& registry, ECS::Entity pawn, float start_y) const {
	const auto& state = registry.GetComponent<ECS::PawnStateComponent>(pawn);
	
	MOMOS::DrawSetFillColor(255, 255, 255, 255);
	MOMOS::DrawSetTextSize(kTextSize - 2.0f);
	
	float x = kTextPadding;
	float y = start_y;
	
	char state_text[128];
	snprintf(state_text, sizeof(state_text), "State: %s", GetStateName(state.status));
	MOMOS::DrawText(x, y, state_text);
}

void InfoPanel::DrawFollowButton(float x, float y) const {
	// Store button bounds for click detection
	follow_button_bounds_[0] = x;
	follow_button_bounds_[1] = y;
	follow_button_bounds_[2] = x + kFollowButtonWidth;
	follow_button_bounds_[3] = y + kFollowButtonHeight;
	
	float points[10] = {
		follow_button_bounds_[0], follow_button_bounds_[1],
		follow_button_bounds_[2], follow_button_bounds_[1],
		follow_button_bounds_[2], follow_button_bounds_[3],
		follow_button_bounds_[0], follow_button_bounds_[3],
		follow_button_bounds_[0], follow_button_bounds_[1]
	};
	
	bool is_following = Camera::IsFollowing();
	unsigned char baseColor = is_following ? 90 : 60;
	unsigned char alpha = is_following ? 200 : 140;
	
	MOMOS::DrawSetFillColor(baseColor, baseColor, baseColor, alpha);
	MOMOS::DrawSolidPath(points, 5);
	MOMOS::DrawSetStrokeColor(220, 220, 220, 180);
	MOMOS::DrawPath(points, 5);
	
	const char* label = is_following ? "Follow: ON" : "Follow: OFF";
	MOMOS::DrawSetFillColor(240, 240, 240, 255);
	MOMOS::DrawSetTextSize(14.0f);
	MOMOS::DrawText(x + 8.0f, y + kFollowButtonHeight / 2.0f + 5.0f, label);
}

bool InfoPanel::IsFollowButtonClicked(float mouse_x, float mouse_y) const {
	return mouse_x >= follow_button_bounds_[0] && mouse_x <= follow_button_bounds_[2] &&
		mouse_y >= follow_button_bounds_[1] && mouse_y <= follow_button_bounds_[3];
}

bool InfoPanel::HandleClick(float mouse_x, float mouse_y) {
	if (!selected_pawn_.IsValid()) {
		return false;
	}
	
	if (IsFollowButtonClicked(mouse_x, mouse_y)) {
		if (Camera::IsFollowing()) {
			Camera::StopFollowing();
		} else {
			Camera::StartFollowing(selected_pawn_);
		}
		return true;
	}
	
	return false;
}

const char* InfoPanel::GetStateName(PawnStatus status) {
	switch (status) {
		case kIdle: return "Idle";
		case kGoingToWork: return "Going To Work";
		case kWorkingLoaded: return "Working (Loaded)";
		case kWorkingUnloaded: return "Working (Unloaded)";
		case kGoingToRest: return "Going To Rest";
		case kResting: return "Resting";
		case kEscaping: return "Escaping";
		case kMoveToProvider: return "Moving To Provider";
		case kWorking: return "Working";
		case kSleeping: return "Sleeping";
		default: return "Unknown";
	}
}

