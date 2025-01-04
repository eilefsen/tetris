#pragma once

#include "block.hpp"
#include "tet.hpp"

struct Collision {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};

Collision check_collision(Block tet[4], std::vector<Block> board);
