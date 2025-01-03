#include <map>
#include <vector>

#include "raylib.h"
#include "tet.hpp"

static const int FPS_TARGET = 60;
static const float BLOCK_SIZE = 32.0;
static const int GRID_HEIGHT = 16;
static const int GRID_WIDTH = 10;
static const int WINDOW_WIDTH = (GRID_WIDTH * BLOCK_SIZE);
static const int WINDOW_HEIGHT = (GRID_HEIGHT * BLOCK_SIZE);

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
			bool xMatch = t.x == b.pos.x;
			if (xMatch && t.y == b.pos.y - 1) {
				col.down = true;
			}
			if (xMatch && t.y == b.pos.y + 1) {
				col.up = true;
			}
			bool yMatch = (t.y == b.pos.y);
			if (yMatch && t.x == b.pos.x - 1) {
				col.right = true;
			}
			if (yMatch && t.x == b.pos.x + 1) {
				col.left = true;
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

int calculate_score(int cleared) {
	switch (cleared) {
	case 1:
		return 50;
	case 2:
		return 200;
	case 3:
		return 400;
	case 4:
		return 800;
	}
	return 0;
}

void draw_blocks(std::vector<Block> blocks) {
	for (const auto &b : blocks) {
		float x = b.pos.x * BLOCK_SIZE;
		float y = b.pos.y * BLOCK_SIZE;
		DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, b.color);
	}
}

int main() {
	// init
	SetTraceLogLevel(LOG_ALL);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);

	std::vector<Block> blocks{};
	Tetramino tet = create_random_tet();

	int score = 0;

	int game_time = 0;
	int frames_per_fall = 40; // reduce this to increase speed and difficulty

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
					score += calculate_score(cleared);
					TraceLog(LOG_INFO, "Cleared %d rows! score: %d\n", cleared, score);
				}
				blocks = total_blocks;
				tet = create_random_tet();
			}
			if (score != 0 && score % 1000 == 0) {
				printf("score: %d, speed: %d", score, frames_per_fall);
				frames_per_fall = std::max(5, frames_per_fall - 5);
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
