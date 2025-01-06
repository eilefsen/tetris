#pragma once

#include <cstdint>
#include <raylib.h>

#include "block.hpp"

// Tetramino constants use relative block coordinates
class Tetramino {
  private:
	uint16_t pattern[4]; // Binary numbers each representing a 4x4 grid rotation
	int x_offset = 3;
	int y_offset = -2;
	size_t pattern_idx = 0;
	std::array<std::array<Coordinate, 5>, 4> rotation_tests{{
		{
			Coordinate{.x = 0, .y = 0},
			Coordinate{.x = 1, .y = 0},
			Coordinate{.x = 0, .y = -1},
			Coordinate{.x = -1, .y = 3},
			Coordinate{.x = 1, .y = 0},
		},
		{
			Coordinate{.x = 0, .y = 0},
			Coordinate{.x = -1, .y = 0},
			Coordinate{.x = 0, .y = 1},
			Coordinate{.x = 1, .y = -3},
			Coordinate{.x = -1, .y = 0},

		},
		{
			Coordinate{.x = 0, .y = 0},
			Coordinate{.x = -1, .y = 0},
			Coordinate{.x = 0, .y = -1},
			Coordinate{.x = 1, .y = 3},
			Coordinate{.x = -1, .y = 0},

		},
		{
			Coordinate{.x = 0, .y = 0},
			Coordinate{.x = 1, .y = 0},
			Coordinate{.x = 0, .y = 1},
			Coordinate{.x = -1, .y = -3},
			Coordinate{.x = 1, .y = 0},

		},
	}};

  public:
	std::array<Block, 4> blocks;
	// `pattern` should only contain 4 ones, will otherwise return early, and log error.
	std::array<Block, 4> create_blocks(uint16_t pattern, Color color);

	void move(int y, int x);

	void fall();
	void left();
	void right();
	size_t rotate(std::vector<Block> board);

	Tetramino(Color color, uint16_t pattern[4]);
	Tetramino(
		Color color, uint16_t pattern[4],
		std::array<std::array<Coordinate, 5>, 4> rotation_tests
	);
};

Tetramino create_i_tet();
Tetramino create_t_tet();
Tetramino create_j_tet();
Tetramino create_l_tet();
Tetramino create_o_tet();
Tetramino create_s_tet();
Tetramino create_z_tet();
Tetramino create_random_tet();
