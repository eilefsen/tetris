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

void move(Tetramino *t, Collision c, std::vector<Block> blocks) {
	// TODO: add key hold
	if (IsKeyPressed(KEY_H) && !c.base.left) {
		t->left();
	}
	if (IsKeyPressed(KEY_L) && !c.base.right) {
		t->right();
	}
	if (IsKeyPressed(KEY_J) && !c.base.down) {
		t->fall();
	}
	// TODO: Write collision logic for rotate
	if (IsKeyPressed(KEY_R)) {
		auto old_blocks = t->blocks;
		int r_idx = t->rotate(0, 0);
		CollisionBase obs = check_obstruction(t->blocks, blocks);
		int i = 0;

		std::array<std::array<Coordinate, 5>, 4> jlstz_tests{{
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

		while (obs.down || obs.left || obs.right || obs.up) {
			auto test = jlstz_tests[r_idx][i];
			t->move(test.x, test.y);

			if (i >= 4) {
				t->rotate_cw(0, 0); // reset rotation index
				t->blocks = old_blocks;
				break;
			}
			obs = check_obstruction(t->blocks, blocks);
			++i;
		}
	}

	game_time -= 8; // This gives the player more time when rotating.
}

bool check_fail(std::array<Block, 4> blocks) {
	for (const auto &b : blocks) {
		if (b.pos.y < -1) {
			return true;
		}
	}
	return false;
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

	uint64_t cycle_count = 0;
	bool fail = false;

	// run
	while (!WindowShouldClose()) {
		// should close checked twice so that game closes in both loops
		// game loop
		while (!WindowShouldClose() && !fail) {
			std::vector<Block> total_blocks = blocks;
			total_blocks.insert(
				total_blocks.begin(), std::begin(tet.blocks), std::end(tet.blocks)
			);

			// update

			col = check_all_collisions(tet, blocks);
			move(&tet, col, blocks);
			col = check_all_collisions(tet, blocks);

			if (game_time != 0 && game_time % frames_per_fall == 0) {
				game_time = 0;
				++cycle_count;
				TraceLog(LOG_INFO, "cycle: %d", cycle_count);
				if (!col.base.down) {
					tet.fall();

				} else {
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
					auto obs = check_obstruction(tet.blocks, blocks);
					if (obs.down || obs.up || obs.left || obs.right) {
						fail = true;
					}

					if (score / 1000UL * difficulty > 0) {
						TraceLog(
							LOG_INFO, "score: %d, speed: %d", score, frames_per_fall
						);
						frames_per_fall = std::max(5, frames_per_fall - 5);
						++difficulty;
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
	}

	// close
	unload_block_texture();
	CloseWindow();

	return 0;
}
