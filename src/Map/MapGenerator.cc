#include "../../include/Map/MapGenerator.h"
#include "../../include/Map/Map.h"
#include "../../include/Map/ResourceTypeManager.h"
#include "../../include/Map/MapResource.h"
#include <algorithm>
#include <cstdlib>
#include <vector>

bool MapGenerator::Load(Map& map, const char *cost_img, const char *terrain_img) {
	static constexpr int kDefaultCols = 80;
	static constexpr int kDefaultRows = 45;
	static constexpr float kDefaultBlockedRatio = 0.3f;

	return GenerateTileMap(map, kDefaultCols, kDefaultRows, kDefaultBlockedRatio);
}

bool MapGenerator::GenerateTileMap(Map& map, int cols, int rows, float blocked_ratio) {
	const int safe_cols = cols > 0 ? cols : 1;
	const int safe_rows = rows > 0 ? rows : 1;
	
	std::vector<std::vector<bool>> tile_walkable;
	std::vector<std::vector<float>> tile_costs;
	
	tile_walkable.assign(safe_cols, std::vector<bool>(safe_rows, true));
	tile_costs.assign(safe_cols, std::vector<float>(safe_rows, 0.0f)); // Initialize all costs to 0.0f

	float clamped_ratio = blocked_ratio;
	if (clamped_ratio < 0.0f) {
		clamped_ratio = 0.0f;
	} else if (clamped_ratio > 1.0f) {
		clamped_ratio = 1.0f;
	}

	const int total_cells = safe_cols * safe_rows;
	const int desired_blocked = static_cast<int>(clamped_ratio * total_cells);

	auto is_reserved_tile = [safe_cols, safe_rows](int x, int y) {
		if (x == 0 && y == 0) {
			return true;
		}
		if (safe_cols > 1 && x == 1 && y == 0) {
			return true;
		}
		if (safe_rows > 1 && x == 0 && y == 1) {
			return true;
		}
		return false;
	};

	int blocked_cells = 0;
	constexpr int kMinLumpSize = 2;
	constexpr int kMaxLumpSize = 8;
	const int kMaxPlacementAttempts = 500;
	const int kFailureThresholdForSingleCell = std::max(1, (safe_cols * safe_rows) / 4);
	const int kMaxTotalFailures = (safe_cols * safe_rows) * 2;
	int total_failure_count = 0;

	while (blocked_cells < desired_blocked && total_failure_count < kMaxTotalFailures) {
		int remaining = desired_blocked - blocked_cells;
		if (remaining <= 0) {
			break;
		}

		bool allow_single_cell = total_failure_count >= kFailureThresholdForSingleCell;
		int lump_min = (remaining >= kMinLumpSize && !allow_single_cell) ? kMinLumpSize : 1;
		if (lump_min > remaining) {
			lump_min = remaining;
		}

		int lump_max = std::min(kMaxLumpSize, remaining);
		if (lump_max < lump_min) {
			lump_max = lump_min;
		}

		int target_size = lump_min;
		if (lump_max > lump_min) {
			target_size += rand() % (lump_max - lump_min + 1);
		}

		bool placed = false;
		for (int placement_attempt = 0; placement_attempt < kMaxPlacementAttempts && !placed; ++placement_attempt) {
			int start_x = rand() % safe_cols;
			int start_y = rand() % safe_rows;
			if (!tile_walkable[start_x][start_y] || is_reserved_tile(start_x, start_y)) {
				continue;
			}

			std::vector<std::vector<bool>> visited(safe_cols, std::vector<bool>(safe_rows, false));
			std::vector<std::pair<int, int>> lump_cells;
			lump_cells.emplace_back(start_x, start_y);
			visited[start_x][start_y] = true;
			std::vector<std::pair<int, int>> frontier = lump_cells;

			while (static_cast<int>(lump_cells.size()) < target_size) {
				std::vector<std::pair<int, int>> candidates;
				const int kDx[4] = { 1, -1, 0, 0 };
				const int kDy[4] = { 0, 0, 1, -1 };
				for (const auto& cell : frontier) {
					for (int dir = 0; dir < 4; ++dir) {
						int nx = cell.first + kDx[dir];
						int ny = cell.second + kDy[dir];
						if (nx < 0 || nx >= safe_cols || ny < 0 || ny >= safe_rows) {
							continue;
						}
						if (is_reserved_tile(nx, ny) || !tile_walkable[nx][ny] || visited[nx][ny]) {
							continue;
						}
						candidates.emplace_back(nx, ny);
					}
				}

				if (candidates.empty()) {
					break;
				}

				auto next_cell = candidates[rand() % candidates.size()];
				lump_cells.push_back(next_cell);
				visited[next_cell.first][next_cell.second] = true;
				frontier.push_back(next_cell);
			}

			if (static_cast<int>(lump_cells.size()) >= lump_min) {
				for (const auto& cell : lump_cells) {
					int x = cell.first;
					int y = cell.second;
					if (tile_walkable[x][y]) {
						tile_walkable[x][y] = false;
						// Assign random cost from {0.5f, 0.75f, 1.0f} for unwalkable cells
						int random_choice = rand() % 3;
						switch (random_choice) {
							case 0:
								tile_costs[x][y] = 0.5f;
								break;
							case 1:
								tile_costs[x][y] = 0.75f;
								break;
							case 2:
							default:
								tile_costs[x][y] = 1.0f;
								break;
						}
						++blocked_cells;
						if (blocked_cells >= desired_blocked) {
							break;
						}
					}
				}
				placed = true;
			}
		}

		if (placed) {
			total_failure_count = 0;
		} else {
			++total_failure_count;
		}
	}

	// Ensure reserved tiles are walkable
	tile_walkable[0][0] = true;
	tile_costs[0][0] = 0.0f;
	if (safe_cols > 1) {
		tile_walkable[1][0] = true;
		tile_costs[1][0] = 0.0f;
	}
	if (safe_rows > 1) {
		tile_walkable[0][1] = true;
		tile_costs[0][1] = 0.0f;
	}

	// Initialize the map with generated data
	map.Initialize(safe_cols, safe_rows, tile_walkable, tile_costs);
	
	// Add tree resources to cells (4% of walkable cells)
	const MapResourceType* tree_type = ResourceTypeManager::Get().GetResourceType("Tree");
	if (tree_type != nullptr && !tree_type->image_names.empty()) {
		int trees_added = 0;
		for (int x = 0; x < safe_cols; ++x) {
			for (int y = 0; y < safe_rows; ++y) {
				// Only add trees to walkable cells (cost == 0.0f)
				if (tile_walkable[x][y] && tile_costs[x][y] == 0.0f) {
					// Check if this cell should have a tree (4% chance)
					// Use deterministic hash based on cell position for consistent randomness
					unsigned int tree_hash = static_cast<unsigned int>(x * 91234567u) ^ static_cast<unsigned int>(y * 45678901u);
					if ((tree_hash % 100) < 2) {
						MapCell* cell = map.getCellAt(x, y);
						if (cell != nullptr) {
							// Add a tree resource to this cell
							cell->resources.push_back(MapResource(tree_type, 1));
							trees_added++;
						}
					}
				}
			}
		}
		// Debug: Print how many trees were added (can be removed later)
		 printf("Added %d tree resources to map\n", trees_added);
	}
	
	return true;
}

void MapGenerator::InitializeSynthetic(Map& map, int width, int height, bool walkable) {
	const int safe_width = (width > 0) ? width : 1;
	const int safe_height = (height > 0) ? height : 1;
	
	std::vector<std::vector<bool>> tile_walkable(safe_width, std::vector<bool>(safe_height, walkable));
	std::vector<std::vector<float>> tile_costs(safe_width, std::vector<float>(safe_height, 0.0f));
	
	map.Initialize(safe_width, safe_height, tile_walkable, tile_costs);
}

