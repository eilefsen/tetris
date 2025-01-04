#include <cstddef>
#include <cstdint>
#include <random>

#include "raylib.h"
#include "tet.hpp"

void Tetramino::create_blocks(uint16_t pattern, Color color) {
	int x = 0;
	size_t block_counter = 0; // should never exceed 3 (max index of `blocks`)

	for (size_t i = 1; i < 16; ++i) {
		int y = i % 4;
		if (y == 0) {
			++x;
		}

		uint16_t bit_selector = 1UL << i;
		if (pattern & bit_selector) {
			Coordinate pos{.x = x + x_offset, .y = y + y_offset};
			blocks[block_counter++] = Block{.pos = pos, .color = color};
			if (block_counter > 3) {
				return;
			}
		}
	}
}

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

void Tetramino::rotate() {
	if (pattern_idx >= 3) {
		pattern_idx = 0;
	} else {
		++pattern_idx;
	}
	TraceLog(LOG_DEBUG, "pattern_idx: %d", pattern_idx);
	create_blocks(pattern[pattern_idx], blocks[0].color);
}

Tetramino::Tetramino(Color color, uint16_t pattern[4])
	: pattern{pattern[0], pattern[1], pattern[2], pattern[3]} {
	create_blocks(pattern[0], color);
};

Tetramino create_i_tet() {
	uint16_t pattern[4] = {
		0b0000'1111'0000'0000,
		0b0010'0010'0010'0010,
		0b0000'0000'1111'0000,
		0b0100'0100'0100'0100,
	};
	return Tetramino(SKYBLUE, pattern);
}
Tetramino create_t_tet() {
	uint16_t pattern[4] = {
		0b0000'0010'0111'0000,
		0b0100'0110'0100'0000,
		0b0111'0010'0000'0000,
		0b0001'0011'0001'0000,
	};
	return Tetramino(PURPLE, pattern);
}

Tetramino create_j_tet() {
	uint16_t pattern[4] = {
		0b0011'0001'0001'0000,
		0b0000'0111'0100'0000,
		0b0010'0010'0011'0000,
		0b0001'0111'0000'0000,
	};
	return Tetramino(BLUE, pattern);
}
Tetramino create_l_tet() {
	uint16_t pattern[4] = {
		0b0001'0001'0011'0000,
		0b0000'0111'0001'0000,
		0b0011'0010'0010'0000,
		0b0100'0111'0000'0000,
	};
	return Tetramino(ORANGE, pattern);
}
Tetramino create_o_tet() {
	uint16_t pattern[4] = {
		0b0000'0011'0011'0000,
		0b0000'0011'0011'0000,
		0b0000'0011'0011'0000,
		0b0000'0011'0011'0000,
	};
	return Tetramino(YELLOW, pattern);
}
Tetramino create_s_tet() {
	uint16_t pattern[4] = {
		0b0110'0011'0000'0000,
		0b0010'0110'0100'0000,
		// repeat
		0b0110'0011'0000'0000,
		0b0010'0110'0100'0000,
	};
	return Tetramino(RED, pattern);
}
Tetramino create_z_tet() {
	uint16_t pattern[4] = {
		0b0011'0110'0000'0000,
		0b0100'0110'0010'0000,
		// repeat
		0b0011'0110'0000'0000,
		0b0100'0110'0010'0000,
	};
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
static std::uniform_int_distribution<int> distribution(0, 6);

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
