//------------------------------------------------------------------------------
// File: MapService.h
// Purpose: Provides access to the game map
//------------------------------------------------------------------------------
#ifndef MAP_SERVICE_H
#define MAP_SERVICE_H

class Map;

/// Service that provides access to the game map
class MapService {
public:
	MapService();
	~MapService();
	
	/// Set the map (takes ownership)
	void SetMap(Map* map);
	
	/// Get the map (non-owning pointer)
	Map* GetMap() { return map_; }
	const Map* GetMap() const { return map_; }
	
	/// Check if map is available
	bool HasMap() const { return map_ != nullptr; }

private:
	Map* map_;
};

#endif // MAP_SERVICE_H

