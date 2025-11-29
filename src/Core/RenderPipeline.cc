//------------------------------------------------------------------------------
// File: RenderPipeline.cc
// Purpose: Implementation of RenderPipeline
//------------------------------------------------------------------------------
#include "../../include/Core/RenderPipeline.h"

RenderPipeline::RenderPipeline() {
	// Initialize all layers as enabled
	layer_enabled_.resize(4, true);
}

RenderPipeline::~RenderPipeline() {
	Clear();
}

void RenderPipeline::RegisterRenderer(Layer layer, std::function<void()> render_func) {
	RenderEntry entry;
	entry.layer = layer;
	entry.render_func = render_func;
	renderers_.push_back(entry);
}

void RenderPipeline::Render() {
	// Render in layer order: Background -> Entities -> Overlay -> UI
	for (Layer layer : { Layer::Background, Layer::Entities, Layer::Overlay, Layer::UI }) {
		if (!IsLayerEnabled(layer)) {
			continue;
		}
		
		for (auto& entry : renderers_) {
			if (entry.layer == layer && entry.render_func) {
				entry.render_func();
			}
		}
	}
}

void RenderPipeline::Clear() {
	renderers_.clear();
}

void RenderPipeline::SetLayerEnabled(Layer layer, bool enabled) {
	int index = LayerToIndex(layer);
	if (index >= 0 && index < static_cast<int>(layer_enabled_.size())) {
		layer_enabled_[index] = enabled;
	}
}

bool RenderPipeline::IsLayerEnabled(Layer layer) const {
	int index = LayerToIndex(layer);
	if (index >= 0 && index < static_cast<int>(layer_enabled_.size())) {
		return layer_enabled_[index];
	}
	return false;
}

int RenderPipeline::LayerToIndex(Layer layer) const {
	switch (layer) {
		case Layer::Background: return 0;
		case Layer::Entities: return 1;
		case Layer::Overlay: return 2;
		case Layer::UI: return 3;
		default: return -1;
	}
}

