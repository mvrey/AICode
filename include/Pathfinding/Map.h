#ifndef __MAP__
#define __MAP__

#include "../../include/config.h"
#include "MapCell.h"
#include <vector>

/// Map class holds the core map data and provides access methods.
/// It manages the cell grid and provides coordinate transformations.
class Map {
	int width_;
	int height_;
	std::vector<std::vector<MapCell*>> cost_map_;
	
	// Tile data (set by MapGenerator, used by reset())
	std::vector<std::vector<bool>> tile_walkable_;
	std::vector<std::vector<float>> tile_costs_;

public:
	Map();
	~Map();
	
	/// Initializes map dimensions and tile data
	/// Called by MapGenerator to set the terrain data
	void Initialize(int width, int height, 
		const std::vector<std::vector<bool>>& walkable,
		const std::vector<std::vector<float>>& costs);
	
	/// Rebuilds the cost_map_ from tile data
	void reset();
	
	// Getters
	int getHeight() const;
	int getWidth() const;
	MapCell* getCellAt(int x, int y);
	const MapCell* getCellAt(int x, int y) const;
	
	/// Returns if the given coordinate is walkable in Map coordinates
	bool isWalkable(::MOMOS::Vec2 position) const;
	
	/// Translates window coordinates to internal map coordinates
	::MOMOS::Vec2 ScreenToMapCoords(MOMOS::Vec2 pos) const;
	
	/// Translates internal map coordinates into window coordinates
	::MOMOS::Vec2 MapToScreenCoords(::MOMOS::Vec2 pos) const;
	
	/// Returns if the given coordinate is walkable in Screen coordinates
	bool isWalkableInScreenCoords(::MOMOS::Vec2 pos) const;
	
	/// Prints the cells of cost_map
	void Print() const;
	
	/// Gets tile walkable data (for MapRenderer)
	const std::vector<std::vector<bool>>& GetTileWalkable() const { return tile_walkable_; }
	
	/// Gets tile costs data (for MapRenderer)
	const std::vector<std::vector<float>>& GetTileCosts() const { return tile_costs_; }
	
	/// Handles clicking on cells and stores the selected cell
	/// @param screen_pos Screen coordinates of the click
	/// @return true if a cell was clicked, false otherwise
	bool HandleCellClick(const ::MOMOS::Vec2& screen_pos);
	
	/// Draws selection box around the currently selected cell
	void DrawCellSelection() const;
	
	/// Gets the currently selected cell position
	/// @return The selected cell position in map coordinates, or (-1, -1) if none selected
	::MOMOS::Vec2 GetSelectedCell() const;
	
	/// Clears the current cell selection
	void ClearCellSelection();
	
private:
	::MOMOS::Vec2 selected_cell_ = { -1.0f, -1.0f }; // Invalid cell position
};

#endif // __MAP__

