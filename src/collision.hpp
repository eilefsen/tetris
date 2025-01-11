#pragma once

#include "block.hpp"
#include "tet.hpp"

struct CollisionBase {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};

struct Collision {
	CollisionBase base;
	CollisionBase rotated;
};

CollisionBase check_collision(std::array<Block, 4> blocks, std::vector<Block> board);

CollisionBase check_obstruction(std::array<Block, 4> blocks, std::vector<Block> board);

Collision check_all_collisions(Tetramino tet, std::vector<Block> board);
