//------------------------------------------------------------------------------
// File: IMapResourceQuery.h
// Purpose: Interface for querying map resources without exposing Map internals
//------------------------------------------------------------------------------
#ifndef I_MAP_RESOURCE_QUERY_H
#define I_MAP_RESOURCE_QUERY_H

#include "../Map/MapResource.h"
#include <vector>
#include <MOMOS/math.h>

/// Interface for querying map resources without exposing Map internals
class IMapResourceQuery {
public:
	virtual ~IMapResourceQuery() = default;
	
	/// Get all resources at the specified cell coordinates
	/// @param x Cell X coordinate
	/// @param y Cell Y coordinate
	/// @return Vector of resources at that cell
	virtual std::vector<MapResource> GetResourcesAt(int x, int y) const = 0;
	
	/// Convert cell coordinates to world/screen coordinates
	/// @param x Cell X coordinate
	/// @param y Cell Y coordinate
	/// @return World/screen position
	virtual ::MOMOS::Vec2 CellToWorld(int x, int y) const = 0;
	
	/// Get a cell at the specified coordinates (for direct access if needed)
	/// @param x Cell X coordinate
	/// @param y Cell Y coordinate
	/// @return Pointer to MapCell, or nullptr if invalid
	virtual class MapCell* GetCellAt(int x, int y) = 0;
	virtual const class MapCell* GetCellAt(int x, int y) const = 0;
	
	/// Get map width in cells
	virtual int GetWidth() const = 0;
	
	/// Get map height in cells
	virtual int GetHeight() const = 0;
};

#endif // I_MAP_RESOURCE_QUERY_H

