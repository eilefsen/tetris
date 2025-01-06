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

  public:
	std::array<Block, 4> blocks;
	// `pattern` should only contain 4 ones, will otherwise return early, and log error.
	std::array<Block, 4> create_blocks(uint16_t pattern, Color color);

	void move(int y, int x);

	void fall();
	void left();
	void right();
	size_t rotate(int x_offset, int y_offset);
	size_t rotate_cw(int x_offset, int y_offset);

	Tetramino(Color color, uint16_t pattern[4]);
};

Tetramino create_i_tet();
Tetramino create_t_tet();
Tetramino create_j_tet();
Tetramino create_l_tet();
Tetramino create_o_tet();
Tetramino create_s_tet();
Tetramino create_z_tet();
Tetramino create_random_tet();
