#include <stdio.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <thread>
#include <cstring>
#include <algorithm>

#include "../include/GameStatus.h"
#include "../include/config.h"
#include "../include/Camera.h"
#include "../include/Pathfinding/astar.h"
#include "../include/UI/SimulationSpeedControls.h"
#include "../include/UI/FpsCounter.h"
#include "../include/UI/VSyncToggle.h"
#include "../include/UI/InfoPanel.h"
#include "../include/ecs/components/PawnComponents.h"
#include "../include/ecs/components/TransformComponent.h"
#include "../include/UI/InfoPanel.h"
#include "../include/ecs/PawnEcsSystems.h"
#include "../include/ecs/PawnFactory.h"
#include "../include/ecs/PawnSelection.h"
#include "../include/Map/ResourceTypeManager.h"
#include "../include/Needs/NeedsConfig.h"
#include "../include/Providers/ResourceProviderRegistry.h"
#include "../include/Core/GameTimeService.h"
#include "../include/Core/MapService.h"
#include "../include/Core/PathfindingService.h"
#include "../include/Core/GameStateService.h"
#include "../include/Core/GameContext.h"
#include "../include/Core/InputManager.h"
#include "../include/Core/UIInputHandler.h"
#include "../include/Core/GameWorldInputHandler.h"
#include "../include/Core/CameraInputHandler.h"
#include "../include/Core/SystemManager.h"
#include "../include/Core/RenderPipeline.h"
#include "../include/Core/LegacyAgentManager.h"
#include "../include/Map/Map.h"
#include "../include/Map/MapGenerator.h"
#include "../include/Map/MapRenderer.h"
#include <MOMOS/momos.h>
#include <MOMOS/draw.h>
#include <MOMOS/input.h>

#include "../include/Agents/Pathfinder.h"


SimulationSpeedControls g_speed_controls;
FpsCounter g_fps_counter;
VSyncToggle g_vsync_toggle;

// Services (created in game() and used throughout)
static GameTimeService* g_time_service = nullptr;
static MapService* g_map_service = nullptr;
static PathfindingService* g_pathfinding_service = nullptr;
static GameStateService* g_state_service = nullptr;
static GameContext* g_game_context = nullptr;
static MapRenderer* g_map_renderer = nullptr;
static InputManager* g_input_manager = nullptr;
static SystemManager* g_system_manager = nullptr;
static RenderPipeline* g_render_pipeline = nullptr;
static LegacyAgentManager* g_legacy_agent_manager = nullptr;

namespace {

} // namespace


/// Process user input
void Input() {
	// Handle keyboard navigation for pawn selection (always processed)
	PawnSelection::HandleKeyboardNavigation();
	
	// Process all input handlers via InputManager
	if (g_input_manager) {
		g_input_manager->ProcessInput();
	}
}


// UpdateAI removed - now handled by LegacyAgentManager via SystemManager


void Draw() {
	MOMOS::DrawBegin();
	MOMOS::DrawClear(200, 200, 200);

	// Render via RenderPipeline
	if (g_render_pipeline) {
		g_render_pipeline->Render();
	}

	MOMOS::DrawEnd();
	MOMOS::WindowFrame();
}


/** Checks and returns if the agents have been created and the simulation has started. **/
bool checkGameStarted() {
	if (!g_state_service) {
		return false;
	}
	
	if (!g_state_service->ArePawnsCreated()) {
		g_state_service->SetPawnsCreated(true);

		// Pathfinder manager should be the first one to update each frame
		if (g_pathfinding_service && g_pathfinding_service->GetPathfinder()) {
			Agent::agents_.push_back(g_pathfinding_service->GetPathfinder());
		}

		const int total_pawns = 100;
		for (int i = 0; i < total_pawns; ++i) {
			PawnECS::SpawnPawn();
		}
	}

	return g_state_service->ArePawnsCreated();
}


/// Main update loop
void Update(double m_iTimeStep) {
	if (!g_time_service || !g_system_manager) {
		return;
	}
	
	double effective_step = g_time_service->GetEffectiveTimeStep(m_iTimeStep);
	g_time_service->AdvanceTime(m_iTimeStep);
	
	// Keep GameStatus synchronized for backward compatibility
	if (GameStatus::get()) {
		GameStatus::get()->game_time = g_time_service->GetGameTime();
		GameStatus::get()->simulation_speed_ = g_time_service->GetSimulationSpeed();
	}
	
	// Update all systems via SystemManager
	g_system_manager->Update(effective_step, g_game_context);
}


void SetupCamera() {
	::MOMOS::Vec2 initial_zoom_focus = {
		static_cast<float>(Screen::width) * 0.5f,
		static_cast<float>(Screen::height) * 0.5f
	};
	Camera::ZoomBy(Camera::kZoomStep * 2.0f, initial_zoom_focus);
}

void CreateServices() {
	// Create services
	g_time_service = new GameTimeService();
	g_map_service = new MapService();
	g_pathfinding_service = new PathfindingService();
	g_state_service = new GameStateService();
	
	// Create game context
	g_game_context = new GameContext(g_time_service, g_map_service, g_pathfinding_service, g_state_service);
}

void LoadStaticData() {
	// Load resource types from JSON
	ResourceTypeManager::Get().LoadFromJSON("data/resource_types.json");
	// Load needs configuration from JSON
	NeedsConfig::Get().LoadFromJSON("data/needs_config.json");
}

void CreateMap() {
	// Create Map and load it
	Map* map = new Map();
	MapGenerator generator;
	if (!generator.Load(*map, "data/map_03_60x44_bw.bmp", "data/map_03_960x704_layoutAB.bmp")) {
		// Handle error - for now just continue
	}
	g_map_service->SetMap(map);
	
	// Create MapRenderer for drawing
	g_map_renderer = new MapRenderer();

	// Register map resources as need providers (after map is loaded)
	ResourceProviderRegistry::Get().RegisterMapResources(*map);

	// Create pathfinder and initialize it
	Pathfinder* pathfinder = new Pathfinder();
	pathfinder->init(map); // Initialize pathfinder with map
	g_pathfinding_service->SetPathfinder(pathfinder);

	GameStatus::get()->map = map; // Store Map* for backward compatibility
	GameStatus::get()->pathfinder_ = pathfinder;
}

void SetupSpeedControls() {
	// Initialize speed controls UI
	g_speed_controls.Initialize();
	// Wire speed controls to time service
	// Note: g_time_service is static, so we can access it directly without capture
	g_speed_controls.SetSpeedChangedCallback(
		[](float speed) {
			if (g_time_service) {
				g_time_service->SetSimulationSpeed(speed);
				GameStatus::get()->simulation_speed_ = speed; // For backward compatibility
			}
		});
}


void SetupInputHandlers() {
	// Create and configure InputManager
	g_input_manager = new InputManager();
	g_input_manager->RegisterHandler(new UIInputHandler(&g_speed_controls, &g_vsync_toggle, &InfoPanel::Get()));
	g_input_manager->RegisterHandler(new GameWorldInputHandler(g_map_service, &InfoPanel::Get()));
	g_input_manager->RegisterHandler(new CameraInputHandler());
}

void CreateSystems() {
	// Create and configure SystemManager
	g_system_manager = new SystemManager();
	// Register systems in priority order (higher = updated first)
	g_system_manager->RegisterSystem(100, [](double delta_time, const GameContext* context) {
		// Camera follow update (highest priority)
		if (context && context->time) {
			float delta_seconds = static_cast<float>(delta_time) / 1000.0f;
			Camera::UpdateFollow(delta_seconds);
		}
		});
	g_system_manager->RegisterSystem(80, [](double delta_time, const GameContext* context) {
		// Legacy Agent AI system
		if (g_legacy_agent_manager && context && context->time && delta_time > 0.0) {
			bool started = checkGameStarted();
			if (started) {
				g_legacy_agent_manager->Update(delta_time, context->time);
			}
		}
		});
	g_system_manager->RegisterSystem(50, [](double delta_time, const GameContext* context) {
		// ECS systems
		PawnECS::Systems::Get().Update(delta_time, context);
		});
}

void CreateRenderPipeline() {
	// Create and configure RenderPipeline
	g_render_pipeline = new RenderPipeline();
	// Background layer - map
	g_render_pipeline->RegisterRenderer(RenderPipeline::Layer::Background, []() {
		if (g_map_renderer && g_map_service && g_map_service->GetMap()) {
			g_map_renderer->Draw(*g_map_service->GetMap());
		}
		});
	// Entities layer - agents and pawns
	g_render_pipeline->RegisterRenderer(RenderPipeline::Layer::Entities, []() {
		// Draw legacy agents
		if (g_legacy_agent_manager) {
			g_legacy_agent_manager->Render();
		}
		// Draw ECS pawns
		PawnECS::Systems::Get().Render(0.0, g_game_context);
		});
	// Overlay layer - selections and indicators
	g_render_pipeline->RegisterRenderer(RenderPipeline::Layer::Overlay, []() {
		PawnSelection::DrawSelection();
		if (g_map_service && g_map_service->GetMap()) {
			g_map_service->GetMap()->DrawCellSelection();
		}
		});
	// UI layer - all UI elements
	g_render_pipeline->RegisterRenderer(RenderPipeline::Layer::UI, []() {
		g_fps_counter.Draw();
		g_vsync_toggle.Draw(g_fps_counter.GetTextRight(), g_fps_counter.GetTextBaselineY());
		g_speed_controls.Draw();
		InfoPanel::Get().Draw();
		});

	MOMOS::DrawSetTextFont("data/medieval.ttf");
	MOMOS::WindowSetMouseVisibility(true);
	MOMOS::DrawSetFillColor(200, 50, 100, 255);
}


void Cleanup() {
	// Cleanup handled in game() destructor section
	delete g_render_pipeline;
	g_render_pipeline = nullptr;
	delete g_system_manager;
	g_system_manager = nullptr;
	delete g_input_manager;
	g_input_manager = nullptr;
	delete g_legacy_agent_manager;
	g_legacy_agent_manager = nullptr;
	delete g_map_renderer;
	g_map_renderer = nullptr;
	delete g_game_context;
	g_game_context = nullptr;
	delete g_state_service;
	g_state_service = nullptr;
	delete g_pathfinding_service;
	g_pathfinding_service = nullptr;
	delete g_map_service; // This will delete the map
	g_map_service = nullptr;
	delete g_time_service;
	g_time_service = nullptr;
}


void InitGame() {
	srand(static_cast<unsigned int>(time(NULL)));

	MOMOS::WindowInit(Screen::width, Screen::height);
	Camera::Initialize();
	SetupCamera();
	g_vsync_toggle.Initialize(false);
	CreateServices();
	LoadStaticData();
	CreateMap();
	CreateMap();

	// Keep GameStatus for backward compatibility (legacy code still uses it)
	// TODO: Remove this once all code is migrated
	GameStatus::get()->game_time = 0.0; // Initialize for backward compatibility
	GameStatus::get()->simulation_speed_ = 1.0f; // Initialize for backward compatibility
	GameStatus::get()->pawns_created = false; // Initialize for backward compatibility

	// Create legacy agent manager
	g_legacy_agent_manager = new LegacyAgentManager();

	SetupSpeedControls();
	SetupInputHandlers();
	CreateSystems();
	CreateRenderPipeline();
}


/// Entry point
int game(int argc, char** argv) {

	InitGame();

	// 40ms per frame
	float m_iTimeStep = 16.0f;
	double CurrentTime = MOMOS::Time();

	checkGameStarted();

	// Game loop with fixed update
	while (MOMOS::WindowIsOpened()) {

		Input();

		g_fps_counter.Update();

		double accumTime = MOMOS::Time() - CurrentTime;
		while (accumTime >= m_iTimeStep) {
			Update(m_iTimeStep);
			CurrentTime += m_iTimeStep;
			accumTime = MOMOS::Time() - CurrentTime;
		}

		Draw();
	}

	Cleanup();

	return 0;
}