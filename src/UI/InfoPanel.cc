#include "../../include/UI/InfoPanel.h"
#include "../../include/ecs/Registry.h"
#include "../../include/ecs/Entity.h"
#include "../../include/ecs/components/NeedsComponent.h"
#include "../../include/ecs/PawnEcs.h"
#include <utility>

#include <MOMOS/draw.h>
#include <algorithm>

InfoPanel& InfoPanel::Get() {
	static InfoPanel instance;
	return instance;
}

InfoPanel::InfoPanel()
	: message_("Nothing to show yet"), selected_pawn_() {
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

void InfoPanel::Draw() const {
	// Determine panel height based on whether we're showing needs
	bool showing_needs = selected_pawn_.IsValid();
	float panel_height = showing_needs ? kPanelHeightWithNeeds : kPanelHeight;
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

	// Draw needs bars if a pawn is selected
	if (showing_needs) {
		// Get registry from PawnECS
		auto& registry = PawnECS::GetRegistry();
		if (registry.HasComponent<ECS::NeedsComponent>(selected_pawn_)) {
			float needs_start_y = panel_top_y + kTextSize + kTextPadding * 2.0f;
			DrawNeedsBars(registry, selected_pawn_, needs_start_y);
		}
	}
}

void InfoPanel::DrawNeedsBars(const ECS::Registry& registry, ECS::Entity pawn, float start_y) const {
	const auto& needs = registry.GetComponent<ECS::NeedsComponent>(pawn);
	
	float x = kTextPadding;
	float y = start_y;
	
	// Draw each need bar
	DrawNeedBar("Hunger:", needs.hunger, x, y, kNeedBarWidth, kNeedBarHeight);
	y += kNeedBarHeight + kNeedBarSpacing;
	
	DrawNeedBar("Energy:", needs.energy, x, y, kNeedBarWidth, kNeedBarHeight);
	y += kNeedBarHeight + kNeedBarSpacing;
	
	DrawNeedBar("Joy:", needs.joy, x, y, kNeedBarWidth, kNeedBarHeight);
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

