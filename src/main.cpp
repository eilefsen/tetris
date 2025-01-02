#include <map>
#include <random>
#include <vector>

#include <raylib.h>

static const int FPS_TARGET = 60;
static const float BLOCK_SIZE = 32.0;
static const int GRID_HEIGHT = 16;
static const int GRID_WIDTH = 10;
static const int WINDOW_WIDTH = (GRID_WIDTH * BLOCK_SIZE);
static const int WINDOW_HEIGHT = (GRID_HEIGHT * BLOCK_SIZE);

struct Coordinate {
	int x; // X coordinate in grid
	int y; // Y coordinate in grid
};

struct Block {
	Coordinate pos;
	Color color;
};
// Tetramino constants use relative block coordinates
class Tetramino {
  private:
	uint16_t pattern[4]; // Binary numbers each representing a 4x4 grid rotation
	int x_offset = 3;
	int y_offset = -1;
	int pattern_idx = 0;

  public:
	Block blocks[4];

	void fall() {
		y_offset += 1;
		for (auto &b : blocks) {
			b.pos.y += 1;
		}
	}
	void left() {
		x_offset -= 1;
		for (auto &b : blocks) {
			b.pos.x -= 1;
		}
	}
	void right() {
		x_offset += 1;
		for (auto &b : blocks) {
			b.pos.x += 1;
		}
	}

	// `pattern` should only contain 4 ones, will otherwise return early, and log error.
	void create_blocks(uint16_t pattern, Color color) {
		int x = 0;
		size_t block_counter = 0; // should never exceed 3 (max index of `blocks`)

		for (uint i = 1; i < 16; ++i) {
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

	void rotate() {
		if (pattern_idx >= 3) {
			pattern_idx = 0;
		} else {
			++pattern_idx;
		}
		TraceLog(LOG_DEBUG, "pattern_idx: %d", pattern_idx);
		create_blocks(pattern[pattern_idx], blocks[0].color);
	}

	Tetramino(Color color, uint16_t pattern[4])
		: pattern{pattern[0], pattern[1], pattern[2], pattern[3]} {
		create_blocks(pattern[0], color);
	}
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
static std::ranlux24 generator;
static std::uniform_int_distribution<int> distribution(0, 6);

Tetramino create_random_tet() {

	// NOTE: no exit condition in the for statement (continued below)
	for (int i = 0;; ++i) {
		if (bag[i]) {
			break;
		}
		if (i > 6) {
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

struct Collision {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};
Collision check_collision(Block tet[4], std::vector<Block> board) {
	Collision col;
	int lowest_y = 0;
	int rightest_x = 0;
	int leftest_x = GRID_HEIGHT - 1;

	for (int i = 0; i < 4; ++i) {
		Coordinate t = tet[i].pos;

		if (t.y > lowest_y) {
			lowest_y = t.y;
		}
		if (t.x > rightest_x) {
			rightest_x = t.x;
		}
		if (t.x < leftest_x) {
			leftest_x = t.x;
		}

		for (const auto &b : board) {
			if (t.x == b.pos.x) {
				if (t.y == b.pos.y - 1) {
					col.down = true;
					break;
				}
				if (t.y == b.pos.y + 1) {
					col.up = true;
					break;
				}
			} else if (t.y == b.pos.y) {
				if (t.x == b.pos.x - 1) {
					col.right = true;
				}
				if (t.x == b.pos.x + 1) {
					col.left = true;
				}
			}
		}
	}

	if (lowest_y >= GRID_HEIGHT - 1) {
		col.down = true;
	}
	if (rightest_x >= GRID_WIDTH - 1) {
		col.right = true;
	}
	if (leftest_x <= 0) {
		col.left = true;
	}

	return col;
}

void move(Tetramino *t, Collision c) {
	// TODO: add key hold

	if (IsKeyPressed(KEY_H) && !c.left) {
		t->left();
	}
	if (IsKeyPressed(KEY_L) && !c.right) {
		t->right();
	}
	if (IsKeyPressed(KEY_J) && !c.down) {
		t->fall();
	}
	// TODO: Write collision logic for rotate
	if (IsKeyPressed(KEY_R)) {
		t->rotate();
	}
}

void draw_blocks(std::vector<Block> blocks);

// Returns a `tuple<int, vector<Block>>`, where the int is the number of lines cleared,
// and the vector is the transformed vector of blocks
std::tuple<int, std::vector<Block>> clear_blocks(std::vector<Block> blocks) {
	int clear_count = 0;
	std::map<int, std::vector<Block>> rows;

	for (const auto &b : blocks) {
		rows[b.pos.y].push_back(b);
	}

	for (int i = 0 - 4; i < GRID_HEIGHT; ++i) {
		if (rows[i].size() >= GRID_WIDTH) {
			rows[i].clear();
			for (int j = i - 1; j > 0; --j) {
				for (auto &b : rows[j]) {
					b.pos.y += 1;
				}
			}
			clear_count++;
		}
	}

	std::vector<Block> out{};
	for (int i = GRID_HEIGHT - 1; i >= 0; --i) {
		out.insert(out.end(), rows[i].begin(), rows[i].end());
	}

	return std::make_tuple(clear_count, out);
}

int main() {
	// init
	SetTraceLogLevel(LOG_ALL);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);

	std::vector<Block> blocks{};
	Tetramino tet = create_random_tet();

	int game_time = 0;
	int frames_per_fall = 60; // reduce this to increase speed and difficulty

	Collision col{};

	// run
	while (!WindowShouldClose()) {
		std::vector<Block> total_blocks = blocks;
		total_blocks.insert(
			total_blocks.begin(), std::begin(tet.blocks), std::end(tet.blocks)
		);
		// update
		if (game_time % frames_per_fall == 0) {
			tet.fall();
			game_time = 0;
			if (col.down) {
				int cleared;

				tie(cleared, total_blocks) = clear_blocks(total_blocks);
				if (cleared > 0) {
					printf("Cleared %d rows!\n", cleared);
				}
				blocks = total_blocks;
				tet = create_random_tet();
			}
		}

		move(&tet, col);
		col = check_collision(tet.blocks, blocks);

		// draw
		BeginDrawing();
		ClearBackground(GRAY);

		draw_blocks(total_blocks);

		EndDrawing();
		++game_time;
	}

	// close
	CloseWindow();

	return 0;
}

void draw_blocks(std::vector<Block> blocks) {
	for (const auto &b : blocks) {
		float x = b.pos.x * BLOCK_SIZE;
		float y = b.pos.y * BLOCK_SIZE;
		DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, b.color);
	}
}
