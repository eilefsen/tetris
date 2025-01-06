#include <format>
#include <vector>

#include "raylib.h"

#include "block.hpp"
#include "collision.hpp"
#include "tet.hpp"

const int FPS_TARGET = 60;

const int WINDOW_HEIGHT_MARGIN = 2 * BLOCK_SIZE;
const int WINDOW_WIDTH_MARGIN = 2 * BLOCK_SIZE;

const int WINDOW_WIDTH = (GRID_WIDTH * BLOCK_SIZE) + WINDOW_WIDTH_MARGIN;
const int WINDOW_HEIGHT = (GRID_HEIGHT * BLOCK_SIZE) + WINDOW_HEIGHT_MARGIN;

const int WINDOW_WIDTH_MARGIN_START = WINDOW_WIDTH - WINDOW_WIDTH_MARGIN;
const int WINDOW_HEIGHT_MARGIN_START = WINDOW_HEIGHT - WINDOW_HEIGHT_MARGIN;

const Rectangle right_margin{
	.width = WINDOW_WIDTH_MARGIN,
	.height = WINDOW_HEIGHT,
	.x = WINDOW_WIDTH_MARGIN_START,
	.y = 0,
};

static int game_time = 0;
static uint score = 0;
static int frames_per_fall = 40; // reduce this to increase speed and difficulty
static uint difficulty = 0;		 // reduce this to increase speed and difficulty

uint calculate_score(int cleared) {
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
		size_t r_idx = t->rotate(0, 0);
		CollisionBase obs = check_obstruction(t->blocks, blocks);
		size_t i = 0;

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
static bool exit_window = false;

void game() {
	uint64_t cycle_count = 0;
	std::vector<Block> blocks{};
	Tetramino tet = create_random_tet();
	Collision col{};

	// game loop
	while (!(exit_window = WindowShouldClose())) {
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
					TraceLog(LOG_INFO, "Cleared %d rows! score: %d\n", cleared, score);
				}
				// fail if placed tet is above 0
				for (size_t i = 0; i < 4; ++i) {
					if (tet.blocks[i].pos.y < 0) {
						return;
					}
				}

				blocks = total_blocks;
				tet = create_random_tet();

				if (score / 1000UL * difficulty > 0) {
					TraceLog(LOG_INFO, "score: %d, speed: %d", score, frames_per_fall);
					frames_per_fall = std::max(5, frames_per_fall - 5);
					++difficulty;
				}
			}
		}

		// TraceLog(LOG_INFO, "frame: %d\n", game_time);
		BeginDrawing();
		ClearBackground(GRAY);
		DrawRectangleRec(right_margin, DARKGRAY);
		// draw dotted line
		for (int i = 0; i < 20; i += 2) {
			int length = WINDOW_WIDTH / 20;
			DrawLineEx(
				Vector2{.x = static_cast<float>(length * i), .y = WINDOW_HEIGHT_MARGIN},
				Vector2{
					.x = static_cast<float>(length * (i + 1)), .y = WINDOW_HEIGHT_MARGIN
				},
				2,
				DARKGRAY
			);
		}
		DrawText(
			&std::format("score:\n{}", score)[0],
			WINDOW_WIDTH_MARGIN_START + 5,
			WINDOW_HEIGHT_MARGIN_START + 24,
			16,
			WHITE
		);
		draw_blocks(total_blocks, 0, WINDOW_HEIGHT_MARGIN);

		EndDrawing();
		++game_time;
	}
}

int main() {
	// init
	SetTraceLogLevel(LOG_ALL);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);
	load_block_texture();

	game();

	while (!(exit_window = WindowShouldClose())) {
		if (IsKeyPressed(KEY_R)) {
			game();
		}
		BeginDrawing();

		int text_x = (WINDOW_WIDTH / 2) - (24 * 3);
		int text_y = (WINDOW_HEIGHT / 2) - 24;
		DrawRectangleRec(
			Rectangle{
				.x = static_cast<float>(text_x - 12),
				.y = static_cast<float>(text_y - 12),
				.width = (24 * 5) + 12,
				.height = 24 * 5
			},
			ColorAlpha(DARKGRAY, 0.4F)
		);
		DrawText(
			&std::format("You lose.\n\n Score:\n  {}", score)[0],
			text_x,
			text_y,
			24,
			WHITE
		);
		EndDrawing();
	}

	// close
	unload_block_texture();
	CloseWindow();

	return 0;
}
