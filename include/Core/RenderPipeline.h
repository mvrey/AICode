//------------------------------------------------------------------------------
// File: RenderPipeline.h
// Purpose: Coordinates rendering order with separate layers
//------------------------------------------------------------------------------
#ifndef RENDER_PIPELINE_H
#define RENDER_PIPELINE_H

#include <vector>
#include <functional>

/// Manages rendering order with separate layers
class RenderPipeline {
public:
	enum class Layer {
		Background,  // Map, terrain
		Entities,    // Agents, pawns
		Overlay,     // UI overlay
		UI           // UI elements
	};
	
	RenderPipeline();
	~RenderPipeline();
	
	/// Register a render function for a specific layer
	/// @param layer The layer to render on
	/// @param render_func Function to call for rendering
	void RegisterRenderer(Layer layer, std::function<void()> render_func);
	
	/// Execute all renderers in layer order
	void Render();
	
	/// Clear all registered renderers
	void Clear();
	
	/// Enable/disable a layer
	void SetLayerEnabled(Layer layer, bool enabled);
	bool IsLayerEnabled(Layer layer) const;

private:
	struct RenderEntry {
		Layer layer;
		std::function<void()> render_func;
	};
	
	std::vector<RenderEntry> renderers_;
	std::vector<bool> layer_enabled_;
	
	int LayerToIndex(Layer layer) const;
};

#endif // RENDER_PIPELINE_H

