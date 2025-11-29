//------------------------------------------------------------------------------
// File: MapResourceProvider.cc
// Purpose: Implementation of MapResourceProvider
//------------------------------------------------------------------------------
#include "../../include/Providers/MapResourceProvider.h"
#include "../../include/Providers/IMapResourceQuery.h"
#include "../../include/Map/MapCell.h"
#include <algorithm>

MapResourceProvider::MapResourceProvider(IMapResourceQuery* map_query, int cell_x, int cell_y, 
	const std::string& resource_type_name, NeedId need_id, 
	float restore_amount, double use_duration)
	: map_query_(map_query)
	, cell_x_(cell_x)
	, cell_y_(cell_y)
	, resource_type_name_(resource_type_name)
	, need_id_(need_id)
	, restore_amount_(restore_amount)
	, use_duration_(use_duration)
{
}

MapResource* MapResourceProvider::GetCurrentResource() const {
	if (!map_query_) {
		return nullptr;
	}

	MapCell* cell = map_query_->GetCellAt(cell_x_, cell_y_);
	if (!cell) {
		return nullptr;
	}

	// Find the resource with matching type name
	for (auto& resource : cell->resources) {
		if (resource.type && resource.type->name == resource_type_name_ && resource.CanUse()) {
			return &resource;
		}
	}

	return nullptr;
}

bool MapResourceProvider::IsAvailable() const {
	MapResource* resource = GetCurrentResource();
	return resource != nullptr;
}

void MapResourceProvider::OnUsed() {
	MapResource* resource = GetCurrentResource();
	if (!resource) {
		return;
	}

	if (!map_query_) {
		return;
	}

	MapCell* cell = map_query_->GetCellAt(cell_x_, cell_y_);
	if (!cell) {
		return;
	}

	// Use the resource (this will produce outputs if configured)
	resource->Use();

	// If the resource type is set to destroy on use, remove it from the cell
	if (resource->type && resource->type->destroy_on_use) {
		// Find and remove this resource from the cell
		auto& resources = cell->resources;
		resources.erase(
			std::remove_if(resources.begin(), resources.end(),
				[resource](const MapResource& r) {
					return &r == resource;
				}),
			resources.end()
		);
	}
}

::MOMOS::Vec2 MapResourceProvider::GetPosition() const {
	if (!map_query_) {
		return { 0.0f, 0.0f };
	}

	// Convert cell coordinates to world/screen coordinates
	return map_query_->CellToWorld(cell_x_, cell_y_);
}

