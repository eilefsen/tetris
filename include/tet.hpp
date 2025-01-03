#pragma once

#include <cstdint>
#include <raylib.h>

#include "block.hpp"

// Tetramino constants use relative block coordinates
class Tetramino {
  private:
	uint16_t pattern[4]; // Binary numbers each representing a 4x4 grid rotation
	int x_offset = 3;
	int y_offset = -1;
	int pattern_idx = 0;
	// `pattern` should only contain 4 ones, will otherwise return early, and log error.
	void create_blocks(uint16_t pattern, Color color);

  public:
	Block blocks[4];

	void fall();
	void left();
	void right();

	void rotate();

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
