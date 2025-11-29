//------------------------------------------------------------------------------
// File: NeedsConfig.cc
// Purpose: Implementation of NeedsConfig JSON loader
//------------------------------------------------------------------------------
#include "../../include/Needs/NeedsConfig.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <cstring>

namespace {

// Simple JSON parser helpers
void SkipWhitespace(const char*& ptr) {
	while (*ptr && std::isspace(static_cast<unsigned char>(*ptr))) {
		++ptr;
	}
}

bool SkipChar(const char*& ptr, char c) {
	SkipWhitespace(ptr);
	if (*ptr == c) {
		++ptr;
		return true;
	}
	return false;
}

std::string ParseString(const char*& ptr) {
	SkipWhitespace(ptr);
	if (*ptr != '"') {
		return "";
	}
	++ptr;
	
	std::string result;
	while (*ptr && *ptr != '"') {
		if (*ptr == '\\' && ptr[1]) {
			++ptr;
			switch (*ptr) {
				case 'n': result += '\n'; break;
				case 't': result += '\t'; break;
				case 'r': result += '\r'; break;
				case '\\': result += '\\'; break;
				case '"': result += '"'; break;
				default: result += *ptr; break;
			}
		} else {
			result += *ptr;
		}
		++ptr;
	}
	if (*ptr == '"') {
		++ptr;
	}
	return result;
}

float ParseFloat(const char*& ptr) {
	SkipWhitespace(ptr);
	
	bool negative = false;
	if (*ptr == '-') {
		negative = true;
		++ptr;
	}
	
	float result = 0.0f;
	while (*ptr && std::isdigit(static_cast<unsigned char>(*ptr))) {
		result = result * 10.0f + static_cast<float>(*ptr - '0');
		++ptr;
	}
	
	if (*ptr == '.') {
		++ptr;
		float fraction = 0.1f;
		while (*ptr && std::isdigit(static_cast<unsigned char>(*ptr))) {
			result += static_cast<float>(*ptr - '0') * fraction;
			fraction *= 0.1f;
			++ptr;
		}
	}
	
	return negative ? -result : result;
}

void SkipValue(const char*& ptr) {
	SkipWhitespace(ptr);
	if (*ptr == '"') {
		++ptr;
		while (*ptr && *ptr != '"') {
			if (*ptr == '\\' && ptr[1]) {
				ptr += 2;
			} else {
				++ptr;
			}
		}
		if (*ptr == '"') ++ptr;
	} else if (*ptr == '{') {
		int depth = 1;
		++ptr;
		while (*ptr && depth > 0) {
			if (*ptr == '{') ++depth;
			else if (*ptr == '}') --depth;
			else if (*ptr == '"') {
				++ptr;
				while (*ptr && *ptr != '"') {
					if (*ptr == '\\' && ptr[1]) ptr += 2;
					else ++ptr;
				}
			}
			if (depth > 0) ++ptr;
		}
	} else if (*ptr == '[') {
		int depth = 1;
		++ptr;
		while (*ptr && depth > 0) {
			if (*ptr == '[') ++depth;
			else if (*ptr == ']') --depth;
			++ptr;
		}
	} else {
		while (*ptr && *ptr != ',' && *ptr != '}' && *ptr != ']' && !std::isspace(static_cast<unsigned char>(*ptr))) {
			++ptr;
		}
	}
}

NeedId ParseNeedId(const std::string& name) {
	if (name == "Hunger") return NeedId::Hunger;
	if (name == "Energy") return NeedId::Energy;
	if (name == "Joy") return NeedId::Joy;
	// Default to Hunger if unknown
	return NeedId::Hunger;
}

} // namespace

NeedsConfig& NeedsConfig::Get() {
	static NeedsConfig instance;
	return instance;
}

bool NeedsConfig::LoadFromJSON(const char* json_path) {
	std::ifstream file(json_path);
	if (!file.is_open()) {
		return false;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	return ParseJSON(content.c_str());
}

bool NeedsConfig::ParseJSON(const char* json_content) {
	if (!json_content) {
		return false;
	}

	const char* ptr = json_content;
	SkipWhitespace(ptr);

	if (!SkipChar(ptr, '{')) {
		return false;
	}

	// Look for "needs" key
	std::string key = ParseString(ptr);
	if (key != "needs") {
		return false;
	}

	if (!SkipChar(ptr, ':')) {
		return false;
	}

	if (!SkipChar(ptr, '{')) {
		return false;
	}

	// Parse each need configuration
	while (*ptr && *ptr != '}') {
		SkipWhitespace(ptr);
		if (*ptr == '}') break;

		std::string need_name = ParseString(ptr);
		if (need_name.empty()) {
			break;
		}

		NeedId need_id = ParseNeedId(need_name);
		NeedConfig config;

		if (!SkipChar(ptr, ':')) {
			break;
		}

		if (!SkipChar(ptr, '{')) {
			break;
		}

		// Parse need properties
		while (*ptr && *ptr != '}') {
			SkipWhitespace(ptr);
			if (*ptr == '}') break;

			std::string prop_key = ParseString(ptr);
			if (prop_key.empty()) {
				break;
			}

			if (!SkipChar(ptr, ':')) {
				break;
			}

			if (prop_key == "threshold") {
				config.threshold = ParseFloat(ptr);
			} else if (prop_key == "default_decay_rate") {
				config.default_decay_rate = ParseFloat(ptr);
			} else {
				SkipValue(ptr);
			}

			SkipWhitespace(ptr);
			if (*ptr == ',') {
				++ptr;
			}
		}

		if (*ptr == '}') {
			++ptr;
		}

		configs_[need_id] = config;

		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}

	return true;
}

NeedConfig NeedsConfig::GetConfig(NeedId need_id) const {
	auto it = configs_.find(need_id);
	if (it != configs_.end()) {
		return it->second;
	}
	// Return default config
	NeedConfig default_config;
	default_config.threshold = 0.3f;
	default_config.default_decay_rate = 0.001f;
	return default_config;
}

