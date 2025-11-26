#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include "../config.h"
#include <string>

class InfoPanel {
public:
	static InfoPanel& Get();

	void SetMessage(std::string text);
	void Draw() const;

private:
	InfoPanel();

	std::string message_;

	static constexpr float kPanelHeight = 40.0f;
	static constexpr float kTextSize = 16.0f;
	static constexpr float kTextPadding = 12.0f;
};

#endif // INFO_PANEL_H

