#pragma once

#include <tuple>
#include <vector>

#include "raylib.h"

const float BLOCK_SIZE = 32.0;
const int GRID_HEIGHT = 16;
const int GRID_WIDTH = 10;

struct Coordinate {
	int x; // X coordinate in grid
	int y; // Y coordinate in grid
};

struct Block {
	Coordinate pos;
	Color color;
};

// Returns a `tuple<int, vector<Block>>`, where the int is the number of lines cleared,
// and the vector is the transformed vector of blocks
std::tuple<int, std::vector<Block>> clear_blocks(std::vector<Block> blocks);

void load_block_texture();
void unload_block_texture();

void draw_blocks(std::vector<Block> blocks);
