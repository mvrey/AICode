#include "../../include/SpritesheetLoader.h"

#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace {

// Simple JSON parser for the specific format used in textures.json
// This is a minimal parser that handles the expected structure:
// { "frames": { "name": { "x": 0, "y": 0, "w": 10, "h": 12 }, ... } }

// Skip whitespace
void SkipWhitespace(const char*& ptr) {
	while (*ptr && std::isspace(static_cast<unsigned char>(*ptr))) {
		++ptr;
	}
}

// Skip a specific character
bool SkipChar(const char*& ptr, char c) {
	SkipWhitespace(ptr);
	if (*ptr == c) {
		++ptr;
		return true;
	}
	return false;
}

// Parse a string (quoted)
std::string ParseString(const char*& ptr) {
	SkipWhitespace(ptr);
	if (*ptr != '"') {
		return "";
	}
	++ptr; // Skip opening quote
	
	std::string result;
	while (*ptr && *ptr != '"') {
		if (*ptr == '\\' && ptr[1]) {
			++ptr; // Skip escape character
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
		++ptr; // Skip closing quote
	}
	return result;
}

// Parse a number
int ParseInt(const char*& ptr) {
	SkipWhitespace(ptr);
	
	bool negative = false;
	if (*ptr == '-') {
		negative = true;
		++ptr;
	}
	
	int result = 0;
	while (*ptr && std::isdigit(static_cast<unsigned char>(*ptr))) {
		result = result * 10 + (*ptr - '0');
		++ptr;
	}
	
	return negative ? -result : result;
}

// Parse a frame object: { "x": 0, "y": 0, "w": 10, "h": 12 }
bool ParseFrame(const char*& ptr, SpriteFrame& frame) {
	if (!SkipChar(ptr, '{')) {
		return false;
	}
	
	frame.x = frame.y = frame.w = frame.h = 0;
	
	while (*ptr && *ptr != '}') {
		SkipWhitespace(ptr);
		if (*ptr == '}') {
			break;
		}
		
		std::string key = ParseString(ptr);
		if (key.empty()) {
			return false;
		}
		
		if (!SkipChar(ptr, ':')) {
			return false;
		}
		
		int value = ParseInt(ptr);
		
		if (key == "x") {
			frame.x = value;
		} else if (key == "y") {
			frame.y = value;
		} else if (key == "w") {
			frame.w = value;
		} else if (key == "h") {
			frame.h = value;
		}
		
		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}
	
	return SkipChar(ptr, '}');
}

// Parse the frames object: { "name": { ... }, ... }
bool ParseFrames(const char*& ptr, std::map<std::string, SpriteFrame>& frames) {
	if (!SkipChar(ptr, '{')) {
		return false;
	}
	
	while (*ptr && *ptr != '}') {
		SkipWhitespace(ptr);
		if (*ptr == '}') {
			break;
		}
		
		std::string name = ParseString(ptr);
		if (name.empty()) {
			return false;
		}
		
		if (!SkipChar(ptr, ':')) {
			return false;
		}
		
		SpriteFrame frame;
		if (!ParseFrame(ptr, frame)) {
			return false;
		}
		
		frames[name] = frame;
		
		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}
	
	return SkipChar(ptr, '}');
}

// Parse the main JSON structure: { "frames": { ... } }
bool ParseJSONContent(const char* json_content, std::map<std::string, SpriteFrame>& frames) {
	const char* ptr = json_content;
	
	if (!SkipChar(ptr, '{')) {
		return false;
	}
	
	while (*ptr && *ptr != '}') {
		SkipWhitespace(ptr);
		if (*ptr == '}') {
			break;
		}
		
		std::string key = ParseString(ptr);
		if (key.empty()) {
			return false;
		}
		
		if (!SkipChar(ptr, ':')) {
			return false;
		}
		
		if (key == "frames") {
			if (!ParseFrames(ptr, frames)) {
				return false;
			}
		} else {
			// Skip unknown keys
			SkipWhitespace(ptr);
			if (*ptr == '{') {
				// Skip object
				int depth = 1;
				++ptr;
				while (*ptr && depth > 0) {
					if (*ptr == '{') ++depth;
					else if (*ptr == '}') --depth;
					++ptr;
				}
			} else if (*ptr == '[') {
				// Skip array
				int depth = 1;
				++ptr;
				while (*ptr && depth > 0) {
					if (*ptr == '[') ++depth;
					else if (*ptr == ']') --depth;
					++ptr;
				}
			}
		}
		
		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}
	
	return true;
}

} // namespace

bool SpritesheetLoader::Load(const char* json_path, const char* spritesheet_path) {
	// Release any previously loaded resources
	Release();
	
	// Load the spritesheet texture
	spritesheet_handle_ = MOMOS::SpriteFromFile(spritesheet_path);
	if (!spritesheet_handle_) {
		return false;
	}
	
	// Parse the JSON file
	if (!ParseJSON(json_path)) {
		MOMOS::SpriteRelease(spritesheet_handle_);
		spritesheet_handle_ = nullptr;
		return false;
	}
	
	// Extract individual sprites
	if (!ExtractSprites()) {
		MOMOS::SpriteRelease(spritesheet_handle_);
		spritesheet_handle_ = nullptr;
		frames_.clear();
		return false;
	}
	
	return true;
}

bool SpritesheetLoader::ParseJSON(const char* json_path) {
	std::ifstream file(json_path);
	if (!file.is_open()) {
		return false;
	}
	
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	file.close();
	
	frames_.clear();
	return ParseJSONContent(content.c_str(), frames_);
}

bool SpritesheetLoader::ExtractSprites() {
	if (!spritesheet_handle_) {
		return false;
	}
	
	sprites_.clear();
	
	for (const auto& pair : frames_) {
		const std::string& name = pair.first;
		const SpriteFrame& frame = pair.second;
		
		// Extract sprite using SubSprite
		MOMOS::SpriteHandle sprite = MOMOS::SubSprite(
			spritesheet_handle_,
			frame.x,
			frame.y,
			frame.w,
			frame.h
		);
		
		if (sprite) {
			sprites_[name] = sprite;
		}
	}
	
	return !sprites_.empty();
}

MOMOS::SpriteHandle SpritesheetLoader::GetSprite(const std::string& name) const {
	auto it = sprites_.find(name);
	if (it != sprites_.end()) {
		return it->second;
	}
	return nullptr;
}

MOMOS::SpriteHandle SpritesheetLoader::GetSprite(const char* name) const {
	if (!name) {
		return nullptr;
	}
	return GetSprite(std::string(name));
}

bool SpritesheetLoader::HasSprite(const std::string& name) const {
	return sprites_.find(name) != sprites_.end();
}

std::vector<std::string> SpritesheetLoader::GetSpriteNames() const {
	std::vector<std::string> names;
	names.reserve(sprites_.size());
	for (const auto& pair : sprites_) {
		names.push_back(pair.first);
	}
	return names;
}

void SpritesheetLoader::Release() {
	// Release all individual sprites
	for (auto& pair : sprites_) {
		MOMOS::SpriteRelease(pair.second);
	}
	sprites_.clear();
	
	// Release the main spritesheet
	if (spritesheet_handle_) {
		MOMOS::SpriteRelease(spritesheet_handle_);
		spritesheet_handle_ = nullptr;
	}
	
	frames_.clear();
}

SpritesheetLoader::~SpritesheetLoader() {
	Release();
}

