#include <cstddef>
#include <cstdint>
#include <random>

#include "collision.hpp"
#include "raylib.h"
#include "tet.hpp"

using std::array;

#define BIT_POSITION(i) 1 << i

// argument `ptn` corresponds to a single pattern (see `Tetramino`)
array<Block, 4> Tetramino::create_blocks(Pattern ptn, Color color) {
	size_t x = 0;
	size_t block_counter = 0; // should never exceed 3 (max index of `blocks`)

	array<Block, 4> new_blocks{
		blocks[0],
		blocks[1],
		blocks[2],
		blocks[3],
	};

	for (size_t i = 1; i < 25; ++i) {
		size_t y = i % 5;
		if (y == 0) {
			++x;
		}

		if (ptn[y][x]) {
			Coordinate pos{
				.x = static_cast<int>(x) + x_offset, .y = static_cast<int>(y) + y_offset
			};
			new_blocks[block_counter++] = Block{.pos = pos, .color = color};
			if (block_counter > 3) {
				return new_blocks;
			}
		}
	}
	return new_blocks; // silent fail???
};

void Tetramino::fall() {
	y_offset += 1;
	for (auto &b : blocks) {
		b.pos.y += 1;
	}
}
void Tetramino::left() {
	x_offset -= 1;
	for (auto &b : blocks) {
		b.pos.x -= 1;
	}
}
void Tetramino::right() {
	x_offset += 1;
	for (auto &b : blocks) {
		b.pos.x += 1;
	}
}

void Tetramino::move(int x, int y) {
	x_offset += x;
	y_offset += y;
	for (auto &b : blocks) {
		b.pos.x += x;
		b.pos.y += y;
	}
}

size_t Tetramino::rotate_internal(
	vector<Block> board, array<Coordinate, 5> o1, array<Coordinate, 5> o2, size_t new_idx
) {
	TraceLog(LOG_DEBUG, "new_idx: %d", new_idx);
	auto old_blocks = blocks;
	size_t old_idx = pattern_idx;
	pattern_idx = new_idx;
	blocks = create_blocks(pattern[pattern_idx], blocks[0].color);
	CollisionBase obs = check_obstruction(blocks, board);

	size_t i = 0;
	while (obs.down || obs.left || obs.right || obs.up) {
		// FIXME: sometimes this latches and makes a piece impossible to rotate
		Coordinate test = {.x = o2[i].x - o1[i].x, .y = o2[i].y - o1[i].y};
		move(test.x, test.y);

		if (i >= 4) {
			blocks = old_blocks;
			pattern_idx = old_idx;
			return pattern_idx;
		}

		blocks = create_blocks(pattern[pattern_idx], blocks[0].color);
		obs = check_obstruction(blocks, board);
		++i;
	}

	TraceLog(LOG_DEBUG, "pattern_idx: %d", pattern_idx);
	return pattern_idx;
}
size_t Tetramino::rotate_cw(vector<Block> board) {
	size_t new_idx = pattern_idx >= 3 ? 0 : pattern_idx + 1;

	auto o2 = rotation_offsets.get_rotation_offset(new_idx);
	auto o1 = rotation_offsets.get_rotation_offset(new_idx >= 3 ? 0 : new_idx + 1);

	return rotate_internal(board, o1, o2, new_idx);
}
size_t Tetramino::rotate_ccw(vector<Block> board) {
	size_t new_idx = pattern_idx <= 0 ? 3 : pattern_idx - 1;

	// flipped from ccw method
	auto o1 = rotation_offsets.get_rotation_offset(new_idx);
	auto o2 = rotation_offsets.get_rotation_offset(new_idx <= 0 ? 3 : new_idx - 1);

	return rotate_internal(board, o1, o2, new_idx);
}

Tetramino::Tetramino(Color color, array<Pattern, 4> pattern)
	: pattern{pattern[0], pattern[1], pattern[2], pattern[3]} {
	blocks = create_blocks(pattern[0], color);
};
Tetramino::Tetramino(
	Color color, array<Pattern, 4> pattern, RotationOffsets rotation_offsets
)
	: pattern{pattern[0], pattern[1], pattern[2], pattern[3]},
	  rotation_offsets{rotation_offsets} {
	blocks = create_blocks(pattern[0], color);
};

Tetramino create_i_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 1}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{1, 1, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};
	// clang-format off
	RotationOffsets offsets {
		.ZERO =  {{{0, 0},  {-1, 0}, {2, 0},  {-1, 0}, {2, 0}}},
		.RIGHT = {{{-1, 0}, {0, 0},  {0, 0},  {0, 1},  {0, -2}}},
		.TWO =   {{{-1, 1}, {1, 1},  {-2, 1}, {1, 0},  {-2, 0}}},
		.LEFT =  {{{0, 1},  {0, 1},  {0, 1},  {0, -1}, {0, 2}}},
	};
	// clang-format on

	return Tetramino(SKYBLUE, pattern, offsets);
}
Tetramino create_t_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 1, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};
	return Tetramino(PURPLE, pattern);
}

Tetramino create_j_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0}},
			{{0, 1, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 0}},
			{{0, 0, 0, 1, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};
	return Tetramino(BLUE, pattern);
}
Tetramino create_l_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};
	return Tetramino(ORANGE, pattern);
}
Tetramino create_o_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};

	// clang-format off
	RotationOffsets offsets{
		.ZERO =  {{{0, 0},   {0, 0},   {0, 0},   {0, 0},   {0, 0}}},
		.RIGHT = {{{0, -1},  {0, -1},  {0, -1},  {0, -1},  {0, -1}}},
		.TWO =   {{{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}},
		.LEFT =  {{{-1, 0},  {-1, 0},  {-1, 0},  {-1, 0},  {-1, 0}}},
	};
	// clang-format on

	return Tetramino(YELLOW, pattern, offsets);
}
Tetramino create_s_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};
	return Tetramino(RED, pattern);
}
Tetramino create_z_tet() {
	array<Pattern, 4> pattern = {{
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 1, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0}},
		}},
		{{
			{{0, 0, 0, 0, 0}},
			{{0, 0, 1, 0, 0}},
			{{0, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 0}},
			{{0, 0, 0, 0, 0}},
		}},
	}};
	return Tetramino(GREEN, pattern);
}
static bool bag[7] = {
	true, // 0: i
	true, // 1: j
	true, // 2: l
	true, // 3: t
	true, // 4: o
	true, // 5: s
	true, // 6: z
};
static std::random_device rd;
static std::ranlux24 generator(rd());
static std::uniform_int_distribution<size_t> distribution(0, 6);

Tetramino create_random_tet() {
	// NOTE: no exit condition in the for statement (continued below)
	for (int i = 0;; ++i) {
		if (bag[i]) {
			break;
		}
		if (i > 6) {
			// reset bag if every bag item is false
			for (auto &b : bag) {
				b = true;
			}
			// NOTE: (cont.) Exit from within the loop instead.
			// In this case i think it is easier to reason with.
			break;
		}
	}

	size_t roll = distribution(generator);
	while (!bag[roll]) {
		roll = distribution(generator);
	};

	bag[roll] = false;

	switch (roll) {
	case 0:
		return create_i_tet();
	case 1:
		return create_j_tet();
	case 2:
		return create_l_tet();
	case 3:
		return create_t_tet();
	case 4:
		return create_o_tet();
	case 5:
		return create_s_tet();
	case 6:
		return create_z_tet();
	}

	return create_i_tet(); // probably not necessary
}
