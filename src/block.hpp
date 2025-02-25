#pragma once

#include <tuple>
#include <vector>

#include "raylib.h"

const int BLOCK_SIZE = 32.0;
const int MEDIUMBLOCK_SIZE = 16.0;
const int TINYBLOCK_SIZE = 8.0;
const int GRID_HEIGHT = 16;
const int GRID_WIDTH = 10;

struct Coordinate {
	int x; // X coordinate in grid
	int y; // Y coordinate in grid
};

struct Block {
	Coordinate pos;
	Color color;

	void draw_pro(
		int x_margin, int y_margin, int x_offset, int y_offset, Texture2D texture,
		float opacity
	);

	void draw(
		int x_margin = 0, int y_margin = 0, int x_offset = 0, int y_offset = 0,
		float opacity = 1
	);
	void draw_medium(
		int x_margin = 0, int y_margin = 0, int x_offset = 0, int y_offset = 0,
		float opacity = 1
	);
	void draw_tiny(
		int x_margin = 0, int y_margin = 0, int x_offset = 0, int y_offset = 0,
		float opacity = 1
	);
};

// Returns a `tuple<int, vector<Block>>`, where the int is the number of lines
// cleared, and the vector is the transformed vector of blocks
std::tuple<int, std::vector<Block>> clear_blocks(std::vector<Block> blocks);

void load_block_texture();
void unload_block_texture();

void draw_blocks(
	std::vector<Block> blocks, int x_margin, int y_margin, float opacity = 1
);
void draw_blocks(
	std::array<Block, 4> blocks, int x_margin, int y_margin, float opacity = 1
);
