#include "../../include/Pathfinding/astar.h"
#include <limits>

AStar::AStar() {}
AStar::AStar(const AStar& orig) {}
AStar::~AStar() {}

bool AStar::PreProcess(CostMap *map) {
	map_ = map;

	// Initialize static memory to cache pointers to cells
	// Dimensions of the matrix are map_width X map_height
	list_open_indexes_ = (MapCell***)malloc(sizeof(MapCell***) * map_->getWidth());
	list_closed_indexes_ = (MapCell***)malloc(sizeof(MapCell***) * map_->getWidth());

	for (int i = 0; i < map_->getWidth(); i++) {
		list_open_indexes_[i] = (MapCell**)malloc(sizeof(MapCell**) * map->getHeight());
		list_closed_indexes_[i] = (MapCell**)malloc(sizeof(MapCell**) * map->getHeight());
		for (int j = 0; j < map->getHeight(); j++) {
			list_open_indexes_[i][j] = nullptr;
			list_closed_indexes_[i][j] = nullptr;
		}
	}
	return true;
}


bool AStar::GeneratePath(MOMOS::Vec2 origin, MOMOS::Vec2 destination, Path *path) {

	MapCell* node_start = map_->getCellAt((int)origin.x, (int)origin.y);
	MapCell* node_goal = map_->getCellAt((int)destination.x, (int)destination.y);

	// The set of currently discovered nodes that are not evaluated yet.
	list_open_.clear();

	// The set of nodes already evaluated.
	list_closed_.clear();

	// Initialize start node values
	if (node_start != nullptr) {
		node_start->f = 0;
		node_start->g = 0;
		node_start->h = 0;
		node_start->parent = nullptr;
	}

	//Put node_start on the OPEN list (check for null first)
	if (node_start == nullptr || node_goal == nullptr) {
		return false; // Invalid start or goal
	}
	list_open_.push_back(node_start);


	//Reset indexes 
	for (int i = 0; i < map_->getWidth(); i++) {
		for (int j = 0; j < map_->getHeight(); j++) {
			list_open_indexes_[i][j] = nullptr;
			list_closed_indexes_[i][j] = nullptr;
		}
	}

	//OTHER LOCAL VARS
	MapCell node_current;
	std::vector<MapCell> successors;
	MapCell node_successor;
	MapCell* existing_cell;



	//While the OPEN list is not empty
	while (list_open_.size() > 0) {

		int x, y;

		//Get the node off the OPEN list with the lowest f and call it node_current
		// Check for null pointers first
		if (list_open_[0] == nullptr) {
			// Remove null pointer and continue
			list_open_.erase(list_open_.begin());
			continue;
		}
		node_current = *list_open_[0];
		int lowest_index = 0;

		//Search for the discovered node with the lowest distance to destination
		for (unsigned int i = 0; i < list_open_.size(); i++) {
			if (list_open_[i] != nullptr && list_open_[i]->f < node_current.f) {
				lowest_index = i;
			}
		}

		//Assign node_current (check for null)
		if (list_open_[lowest_index] == nullptr) {
			// Remove null pointer and continue
			list_open_.erase(list_open_.begin() + lowest_index);
			continue;
		}
		node_current = *list_open_[lowest_index];
		//Remove element (check for null before swapping)
		if (list_open_.size() > 1) {
			list_open_[lowest_index] = list_open_[list_open_.size() - 1];
		}
		list_open_.resize(list_open_.size() - 1);




		//If node_current is the same state as node_goal : break from the while loop
		if (node_current.position_.x == node_goal->position_.x && node_current.position_.y == node_goal->position_.y)
			break;



		//Generate each state node_successor that can come after node_current
		successors.clear();

		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				if (i == 0 && j == 0)
					continue;
				if (i != 0 && j != 0)
					continue;

				x = static_cast<int>(node_current.position_.x) + j;
				y = static_cast<int>(node_current.position_.y) + i;

				if (x < 0 || y < 0 || x >= map_->getWidth() || y >= map_->getHeight())
					continue;

				MapCell* cell = map_->getCellAt(x, y);
				if (cell == nullptr || !cell->isWalkable())
					continue;

				successors.push_back(*cell);
			}
		}


		// For each node_successor of node_current
		for (unsigned int i = 0; i < successors.size(); i++) {

		node_successor = successors[i];

		// Get the actual cell to access its cost
		MapCell* successor_cell = map_->getCellAt((int)node_successor.position_.x, (int)node_successor.position_.y);
		if (successor_cell == nullptr || successor_cell->isWalkable() == false) {
				// Skip cells with infinite cost (non-walkable)
				continue;
			}

			// G = cost to move to this cell (using the cell's cost_)
			node_successor.g = static_cast<int>(successor_cell->cost_ * 10.0f); // Scale cost to match integer g values

			// Set the cost of node_successor to be the cost of node_current plus the cost to get to node_successor from node_current 
			node_successor.f = node_current.f + node_successor.g;

			bool goto_next_succesor = false;

			// Find node_successor on the OPEN list 
			if (list_open_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y] != nullptr) {

				existing_cell = list_open_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y];
				// If existing one is as good or better then discard this successor and continue with next successor
				if (existing_cell->f <= node_successor.f) {
					goto_next_succesor = true;
				}			
			}
			if (goto_next_succesor)
				continue;

			// Find node_successor on the CLOSED list 
			if (list_closed_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y] != nullptr) {

				existing_cell = list_closed_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y];
				// Assume existing one is always better than this successor
				goto_next_succesor = true;
			}
			if (goto_next_succesor)
				continue;


			// Remove occurrences of node_successor from OPEN
			if (list_open_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y] != nullptr) {
				bool found = false;
				for (unsigned int h = 0; h < list_open_.size() && !found; h++) {
					existing_cell = list_open_[h];
					if (existing_cell == nullptr) {
						continue; // Skip null pointers
					}
					if (existing_cell->position_.x == node_successor.position_.x && existing_cell->position_.y == node_successor.position_.y) {
						//Swap the element to be removed for the last one in the vector, and shrink its size by 1
						if (list_open_.size() > 1) {
							list_open_[h] = list_open_[list_open_.size() - 1];
						}
						list_open_.resize(list_open_.size() - 1);
						//Update index
						list_open_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y] = nullptr;
						found = true;
					}
				}
			}


			// Remove occurrences of node_successor from CLOSED
			if (list_closed_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y] != nullptr) {
				bool found = false;
				for (unsigned int h = 0; h < list_closed_.size() && !found; h++) {
					existing_cell = list_closed_[h];
					if (existing_cell->position_.x == node_successor.position_.x && existing_cell->position_.y == node_successor.position_.y) {
						//Swap the element to be removed for the last one in the vector, and shrink its size by 1
						list_closed_[h] = list_closed_[list_closed_.size() - 1];
						list_closed_.resize(list_closed_.size() - 1);
						//Update index
						list_closed_indexes_[(int)node_successor.position_.x][(int)node_successor.position_.y] = nullptr;
						found = true;
					}
				}
			}



			// Set the parent of node_successor to node_current 
			node_successor.parent = map_->getCellAt((int)node_current.position_.x, (int)node_current.position_.y);





			// Set h to be the estimated distance to node_goal(using the heuristic function) 
			x = (int)node_successor.position_.x;
			y = (int)node_successor.position_.y;
			node_successor.h = abs((int)node_goal->position_.x - x) + abs((int)node_goal->position_.y - y);


		// Add node_successor to the OPEN list
		MapCell *true_successor = map_->getCellAt((int)node_successor.position_.x, (int)node_successor.position_.y);
		if (true_successor == nullptr) {
			continue; // Skip if cell is null
		}
		true_successor->parent = node_successor.parent;
		true_successor->f = node_successor.f;
		true_successor->g = node_successor.g;
		true_successor->h = node_successor.h;
		list_open_.push_back(true_successor);
		//Update index
		list_open_indexes_[(int)true_successor->position_.x][(int)true_successor->position_.y] = true_successor;

		}


		//Add node_current to the CLOSED list
		list_closed_.push_back(map_->getCellAt((int)node_current.position_.x, (int)node_current.position_.y));
		//Update index
		list_closed_indexes_[(int)node_current.position_.x][(int)node_current.position_.y] = &node_current;
	}


	list_closed_.push_back(node_goal);
	//Update index
	list_closed_indexes_[(int)node_goal->position_.x][(int)node_goal->position_.y] = node_goal;


	/**************************/
	/** GET PATH FROM NODES ***/
	/**************************/

	//Reset path
	path->path_.clear();
	path->current_point_ = 0;

	//Find destination node
	MapCell* destination_node = nullptr;
	bool found = false;
	if (list_closed_indexes_[(int)destination.x][(int)destination.y] != nullptr) {		
		destination_node = list_closed_indexes_[(int)destination.x][(int)destination.y];
	}

	//Go back from destination_node through its parents to find the origin_node
	MapCell* origin_node = destination_node;
	path->Add(destination);
	while (origin_node->parent != nullptr) {
		origin_node = origin_node->parent;
		path->Add(origin_node->position_);
	}

	std::reverse(path->path_.begin(), path->path_.end());
	return false;
}