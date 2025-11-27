#ifndef SPRITESHEET_LOADER_H
#define SPRITESHEET_LOADER_H

#include <MOMOS/sprite.h>
#include <string>
#include <map>
#include <vector>

/// Structure to hold frame information from JSON
struct SpriteFrame {
	int x;
	int y;
	int w;
	int h;
};

/// SpritesheetLoader loads textures from a spritesheet using JSON metadata.
/// It parses the JSON file, loads the main spritesheet texture, and extracts
/// individual sprites using SubSprite.
class SpritesheetLoader {
public:
	/// Loads the spritesheet and parses the JSON file.
	/// @param json_path Path to the JSON file (e.g., "data/textures.json")
	/// @param spritesheet_path Path to the spritesheet image (e.g., "data/textures.png")
	/// @return true if loading was successful, false otherwise
	bool Load(const char* json_path, const char* spritesheet_path);

	/// Gets a sprite handle by name (e.g., "Grass1.png")
	/// @param name The name of the sprite as specified in the JSON
	/// @return SpriteHandle if found, nullptr otherwise
	MOMOS::SpriteHandle GetSprite(const std::string& name) const;

	/// Gets a sprite handle by name (C-string version)
	/// @param name The name of the sprite as specified in the JSON
	/// @return SpriteHandle if found, nullptr otherwise
	MOMOS::SpriteHandle GetSprite(const char* name) const;

	/// Checks if a sprite with the given name exists
	/// @param name The name of the sprite
	/// @return true if the sprite exists, false otherwise
	bool HasSprite(const std::string& name) const;

	/// Gets all loaded sprite names
	/// @return Vector of sprite names
	std::vector<std::string> GetSpriteNames() const;

	/// Releases all loaded sprites and cleans up resources
	void Release();

	/// Destructor - automatically releases resources
	~SpritesheetLoader();

private:
	/// Parses the JSON file and extracts frame information
	/// @param json_path Path to the JSON file
	/// @return true if parsing was successful, false otherwise
	bool ParseJSON(const char* json_path);

	/// Extracts individual sprites from the loaded spritesheet
	/// @return true if extraction was successful, false otherwise
	bool ExtractSprites();

	MOMOS::SpriteHandle spritesheet_handle_;  ///< Handle to the main spritesheet texture
	std::map<std::string, SpriteFrame> frames_;  ///< Map of frame names to their coordinates
	std::map<std::string, MOMOS::SpriteHandle> sprites_;  ///< Map of sprite names to their handles
};

#endif // SPRITESHEET_LOADER_H

