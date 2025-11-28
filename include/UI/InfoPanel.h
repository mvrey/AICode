#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include "../config.h"
#include "../ecs/Entity.h"
#include "../Map/MapResource.h"
#include <string>
#include <vector>

namespace ECS {
	class Registry;
}

class InfoPanel {
public:
	static InfoPanel& Get();

	void SetMessage(std::string text);
	void SetSelectedPawn(ECS::Entity pawn);
	void SetSelectedCellResources(const std::vector<MapResource>& resources);
	void Draw() const;

private:
	InfoPanel();

	void DrawNeedsBars(const ECS::Registry& registry, ECS::Entity pawn, float start_y) const;
	void DrawNeedBar(const char* name, float value, float x, float y, float width, float height) const;
	void DrawResourcesList(float start_y) const;

	std::string message_;
	ECS::Entity selected_pawn_;
	std::vector<MapResource> selected_cell_resources_;

	static constexpr float kPanelHeight = 40.0f;
	static constexpr float kPanelHeightWithNeeds = 140.0f; // Expanded height when showing needs
	static constexpr float kPanelHeightWithResources = 200.0f; // Expanded height when showing resources
	static constexpr float kTextSize = 16.0f;
	static constexpr float kTextPadding = 12.0f;
	static constexpr float kNeedBarHeight = 18.0f;
	static constexpr float kNeedBarSpacing = 4.0f;
	static constexpr float kNeedBarWidth = 200.0f;
	static constexpr float kNeedLabelWidth = 80.0f;
	static constexpr float kResourceLineSpacing = 18.0f;
};

#endif // INFO_PANEL_H

