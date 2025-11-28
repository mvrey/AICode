#include "../../include/Map/ResourceTypeManager.h"
#include "../../include/Map/MapResourceType.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <cstring>

namespace {

// Simple JSON parser helpers (similar to SpritesheetLoader)
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

bool ParseBool(const char*& ptr) {
	SkipWhitespace(ptr);
	if (strncmp(ptr, "true", 4) == 0) {
		ptr += 4;
		return true;
	} else if (strncmp(ptr, "false", 5) == 0) {
		ptr += 5;
		return false;
	}
	return false;
}

// Skip a value (object, array, string, number, bool, null)
void SkipValue(const char*& ptr) {
	SkipWhitespace(ptr);
	if (*ptr == '"') {
		// Skip string
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
	} else {
		// Skip number, bool, or null
		while (*ptr && *ptr != ',' && *ptr != '}' && *ptr != ']' && !std::isspace(*ptr)) {
			++ptr;
		}
	}
}

// Parse an array of strings
bool ParseStringArray(const char*& ptr, std::vector<std::string>& result) {
	if (!SkipChar(ptr, '[')) {
		return false;
	}
	
	result.clear();
	
	while (*ptr && *ptr != ']') {
		SkipWhitespace(ptr);
		if (*ptr == ']') {
			break;
		}
		
		std::string str = ParseString(ptr);
		if (!str.empty()) {
			result.push_back(str);
		}
		
		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}
	
	return SkipChar(ptr, ']');
}

// Parse an array of two integers [min, max]
bool ParseIntPair(const char*& ptr, int& min_val, int& max_val) {
	if (!SkipChar(ptr, '[')) {
		return false;
	}
	
	min_val = ParseInt(ptr);
	SkipWhitespace(ptr);
	if (*ptr == ',') {
		++ptr;
	}
	max_val = ParseInt(ptr);
	
	return SkipChar(ptr, ']');
}

} // namespace

ResourceTypeManager& ResourceTypeManager::Get() {
	static ResourceTypeManager instance;
	return instance;
}

bool ResourceTypeManager::LoadFromJSON(const char* json_path) {
	std::ifstream file(json_path);
	if (!file.is_open()) {
		return false;
	}
	
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	file.close();
	
	resource_types_.clear();
	return ParseJSON(content.c_str());
}

bool ResourceTypeManager::ParseJSON(const char* json_content) {
	const char* ptr = json_content;
	
	if (!SkipChar(ptr, '{')) {
		return false;
	}
	
	// Find "resource_types" key
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
		
		if (key == "resource_types") {
			// Parse resource_types object
			if (!SkipChar(ptr, '{')) {
				return false;
			}
			
			while (*ptr && *ptr != '}') {
				SkipWhitespace(ptr);
				if (*ptr == '}') {
					break;
				}
				
				std::string type_name = ParseString(ptr);
				if (type_name.empty()) {
					return false;
				}
				
				if (!SkipChar(ptr, ':')) {
					return false;
				}
				
				if (!ParseResourceType(ptr, type_name)) {
					return false;
				}
				
				SkipWhitespace(ptr);
				if (*ptr == ',') {
					++ptr;
				}
			}
			
			if (!SkipChar(ptr, '}')) {
				return false;
			}
		} else {
			// Skip unknown keys
			SkipValue(ptr);
		}
		
		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}
	
	// Parse outputs in a second pass (stored temporarily as name->range)
	std::map<std::string, std::map<std::string, std::pair<int, int>>> output_names_map;
	
	// Re-parse to capture output names
	const char* ptr2 = json_content;
	SkipChar(ptr2, '{');
	while (*ptr2 && *ptr2 != '}') {
		SkipWhitespace(ptr2);
		std::string key = ParseString(ptr2);
		if (key == "resource_types") {
			SkipChar(ptr2, ':');
			SkipChar(ptr2, '{');
			while (*ptr2 && *ptr2 != '}') {
				SkipWhitespace(ptr2);
				if (*ptr2 == '}') break;
				std::string type_name = ParseString(ptr2);
				SkipChar(ptr2, ':');
				SkipChar(ptr2, '{');
				
				std::map<std::string, std::pair<int, int>> outputs;
				while (*ptr2 && *ptr2 != '}') {
					SkipWhitespace(ptr2);
					if (*ptr2 == '}') break;
					std::string field = ParseString(ptr2);
					SkipChar(ptr2, ':');
					
					if (field == "outputs") {
						SkipChar(ptr2, '{');
						while (*ptr2 && *ptr2 != '}') {
							SkipWhitespace(ptr2);
							if (*ptr2 == '}') break;
							std::string output_name = ParseString(ptr2);
							SkipChar(ptr2, ':');
							int min_val, max_val;
							if (ParseIntPair(ptr2, min_val, max_val)) {
								outputs[output_name] = {min_val, max_val};
							}
							SkipWhitespace(ptr2);
							if (*ptr2 == ',') ++ptr2;
						}
						SkipChar(ptr2, '}');
					} else {
						SkipValue(ptr2);
					}
					SkipWhitespace(ptr2);
					if (*ptr2 == ',') ++ptr2;
				}
				output_names_map[type_name] = outputs;
				SkipChar(ptr2, '}');
				SkipWhitespace(ptr2);
				if (*ptr2 == ',') ++ptr2;
			}
			break;
		} else {
			SkipValue(ptr2);
		}
		SkipWhitespace(ptr2);
		if (*ptr2 == ',') ++ptr2;
	}
	
	// Resolve output references (convert names to pointers)
	for (auto& pair : resource_types_) {
		MapResourceType& type = pair.second;
		type.resource_outputs.clear();
		
		auto it = output_names_map.find(type.name);
		if (it != output_names_map.end()) {
			for (const auto& output_pair : it->second) {
				const std::string& output_name = output_pair.first;
				const auto& range = output_pair.second;
				
				const MapResourceType* output_type = GetResourceType(output_name);
				if (output_type) {
					type.resource_outputs[output_type] = range;
				}
			}
		}
	}
	
	return true;
}

bool ResourceTypeManager::ParseResourceType(const char*& ptr, const std::string& type_name) {
	if (!SkipChar(ptr, '{')) {
		return false;
	}
	
	MapResourceType resource_type;
	resource_type.name = type_name;
	
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
		
		if (key == "name") {
			resource_type.name = ParseString(ptr);
		} else if (key == "image_names") {
			ParseStringArray(ptr, resource_type.image_names);
		} else if (key == "destroy_on_use") {
			resource_type.destroy_on_use = ParseBool(ptr);
		} else if (key == "max_stack") {
			resource_type.max_stack = ParseInt(ptr);
		} else if (key == "outputs") {
			// Outputs will be parsed in second pass, skip for now
			SkipValue(ptr);
		} else {
			// Skip unknown fields
			SkipValue(ptr);
		}
		
		SkipWhitespace(ptr);
		if (*ptr == ',') {
			++ptr;
		}
	}
	
	resource_types_[type_name] = resource_type;
	return SkipChar(ptr, '}');
}

const MapResourceType* ResourceTypeManager::GetResourceType(const std::string& name) const {
	auto it = resource_types_.find(name);
	if (it != resource_types_.end()) {
		return &it->second;
	}
	return nullptr;
}

bool ResourceTypeManager::HasResourceType(const std::string& name) const {
	return resource_types_.find(name) != resource_types_.end();
}

void ResourceTypeManager::Clear() {
	resource_types_.clear();
}

