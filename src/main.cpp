#include <vector>

#include "raylib.h"

#include "block.hpp"
#include "collision.hpp"
#include "tet.hpp"

const int FPS_TARGET = 60;
const int WINDOW_WIDTH = (GRID_WIDTH * BLOCK_SIZE);
const int WINDOW_HEIGHT = (GRID_HEIGHT * BLOCK_SIZE);

static int game_time = 0;
static int score = 0;
static int frames_per_fall = 40; // reduce this to increase speed and difficulty
static int difficulty = 0;		 // reduce this to increase speed and difficulty

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
		game_time -= 8; // This gives the player more time when rotating.
	}
}

int main() {
	// init
	SetTraceLogLevel(LOG_ALL);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);
	load_block_texture();

	std::vector<Block> blocks{};
	Tetramino tet = create_random_tet();

	Collision col{};

	bool place_flag = false;
	uint64_t cycle_count = 0;

	// run
	while (!WindowShouldClose()) {
		std::vector<Block> total_blocks = blocks;
		total_blocks.insert(
			total_blocks.begin(), std::begin(tet.blocks), std::end(tet.blocks)
		);

		// update

		col = check_collision(tet.blocks, blocks);
		move(&tet, col);
		col = check_collision(tet.blocks, blocks);

		if (game_time != 0 && game_time % frames_per_fall == 0) {
			++cycle_count;
			TraceLog(LOG_INFO, "cycle: %d\n", cycle_count);
			if (!col.down) {
				tet.fall();
			}
			game_time = 0;
			if (col.down) {
				if (!place_flag) {
					place_flag = true;
				} else {
					place_flag = false;
					int cleared;
					tie(cleared, total_blocks) = clear_blocks(total_blocks);
					if (cleared > 0) {
						score += calculate_score(cleared);
						TraceLog(
							LOG_INFO, "Cleared %d rows! score: %d\n", cleared, score
						);
					}
					blocks = total_blocks;
					tet = create_random_tet();
					if (score / 1000UL * difficulty > 0) {
						printf("score: %d, speed: %d", score, frames_per_fall);
						frames_per_fall = std::max(5, frames_per_fall - 5);
						++difficulty;
					}
				}
			}
		}

		// TraceLog(LOG_INFO, "frame: %d\n", game_time);
		BeginDrawing();
		ClearBackground(GRAY);

		draw_blocks(total_blocks);

		EndDrawing();
		++game_time;
	}

	// close
	unload_block_texture();
	CloseWindow();

	return 0;
}
