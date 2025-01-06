#pragma once

#include <cstdint>
#include <raylib.h>

#include "block.hpp"

using std::array, std::vector;

struct RotationOffsets {
	// clang-format off
	array<Coordinate,5> ZERO =  {{{0, 0}, {0, 0},  {0, 0},   {0, 0},  {0, 0}}};
	array<Coordinate,5> RIGHT = {{{0, 0}, {1, 0},  {1, -1},  {0, 2},  {1, 2}}};
	array<Coordinate,5> TWO =   {{{0, 0}, {0, 0},  {0, 0},   {0, 0},  {0, 0}}};
	array<Coordinate,5> LEFT =  {{{0, 0}, {-1, 0}, {-1, -1}, {0, 2},  {-1, 2}}};
	// clang-format on
	array<Coordinate, 5> get_rotation_offset(size_t i) {
		switch (i) {
		case 0:
			return ZERO;
		case 1:
			return LEFT;
		case 2:
			return TWO;
		case 3:
			return RIGHT;
		};
		return ZERO;
	}
};

typedef array<array<bool, 5>, 5> Pattern;

// Tetramino constants use relative block coordinates
class Tetramino {
  private:
	array<Pattern, 4> pattern;
	int x_offset = 3;
	int y_offset = -2;
	size_t pattern_idx = 0;
	RotationOffsets rotation_offsets{};
	size_t rotate_internal(
		vector<Block> board, array<Coordinate, 5> o1, array<Coordinate, 5> o2,
		size_t new_idx
	);

  public:
	array<Block, 4> blocks;
	// `pattern` should only contain 4 ones, will otherwise return early, and log
	// error.
	array<Block, 4> create_blocks(Pattern pattern, Color color);

	void move(int y, int x);

	void fall();
	void left();
	void right();
	size_t rotate_cw(vector<Block> board);
	size_t rotate_ccw(vector<Block> board);

	Tetramino(Color color, array<Pattern, 4> pattern);
	Tetramino(Color color, array<Pattern, 4> pattern, RotationOffsets rotation_offsets);
};

Tetramino create_i_tet();
Tetramino create_t_tet();
Tetramino create_j_tet();
Tetramino create_l_tet();
Tetramino create_o_tet();
Tetramino create_s_tet();
Tetramino create_z_tet();
Tetramino create_random_tet();
