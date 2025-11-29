//------------------------------------------------------------------------------
// File: MapResourceProvider.h
// Purpose: Adapter that wraps MapResource as an INeedProvider
//------------------------------------------------------------------------------
#ifndef MAP_RESOURCE_PROVIDER_H
#define MAP_RESOURCE_PROVIDER_H

#include "INeedProvider.h"
#include "IMapResourceQuery.h"
#include "../Map/MapResource.h"
#include "../Map/MapResourceType.h"
#include "../Needs/NeedId.h"
#include <MOMOS/math.h>

/// Adapter that makes a MapResource on a MapCell act as an INeedProvider
/// This allows resources on the map to satisfy pawn needs
/// Uses cell coordinates instead of pointers to survive map resets
class MapResourceProvider : public INeedProvider {
public:
	/// Create a provider from a MapResource on a MapCell
	/// @param map_query Interface for querying map resources
	/// @param cell_x X coordinate of the cell
	/// @param cell_y Y coordinate of the cell
	/// @param resource_type_name Name of the resource type (for lookup)
	/// @param need_id The need this resource satisfies
	/// @param restore_amount How much this resource restores (0.0 to 1.0)
	/// @param use_duration How long it takes to use this resource (seconds)
	MapResourceProvider(IMapResourceQuery* map_query, int cell_x, int cell_y, 
		const std::string& resource_type_name, NeedId need_id, 
		float restore_amount, double use_duration);

	NeedId GetNeedId() const override { return need_id_; }
	float GetRestoreAmount() const override { return restore_amount_; }
	double GetUseDuration() const override { return use_duration_; }
	bool IsAvailable() const override;
	void OnUsed() override;
	::MOMOS::Vec2 GetPosition() const override;

private:
	IMapResourceQuery* map_query_;
	int cell_x_;
	int cell_y_;
	std::string resource_type_name_;
	NeedId need_id_;
	float restore_amount_;
	double use_duration_;

	/// Get the current resource from the map (looks up by coordinates)
	MapResource* GetCurrentResource() const;
};

#endif // MAP_RESOURCE_PROVIDER_H

