#include "../../include/UI/InfoPanel.h"
#include <utility>

#include <MOMOS/draw.h>

InfoPanel& InfoPanel::Get() {
	static InfoPanel instance;
	return instance;
}

InfoPanel::InfoPanel()
	: message_("Nothing to show yet") {
}

void InfoPanel::SetMessage(std::string text) {
	if (text.empty()) {
		message_ = "Nothing to show yet";
	} else {
		message_ = std::move(text);
	}
}

void InfoPanel::Draw() const {
	float panel_width = static_cast<float>(Screen::width);
	float panel_height = kPanelHeight;
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

	MOMOS::DrawSetFillColor(255, 255, 255, 255);
	MOMOS::DrawSetTextSize(kTextSize);
	MOMOS::DrawText(kTextPadding, panel_top_y + kTextPadding + 2.0f, message_.c_str());
}

