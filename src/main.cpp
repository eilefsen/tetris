#include <format>
#include <vector>

#include "raylib.h"

#include "block.hpp"
#include "collision.hpp"
#include "tet.hpp"
using std::vector;

const int FPS_TARGET = 60;

const int WINDOW_HEIGHT_MARGIN = 2 * BLOCK_SIZE;
const int WINDOW_WIDTH_MARGIN = (2 * BLOCK_SIZE) + (BLOCK_SIZE / 2);

const int WINDOW_WIDTH = (GRID_WIDTH * BLOCK_SIZE) + WINDOW_WIDTH_MARGIN;
const int WINDOW_HEIGHT = (GRID_HEIGHT * BLOCK_SIZE) + WINDOW_HEIGHT_MARGIN;

const int WINDOW_WIDTH_MARGIN_START = WINDOW_WIDTH - WINDOW_WIDTH_MARGIN;
const int WINDOW_HEIGHT_MARGIN_START = WINDOW_HEIGHT - WINDOW_HEIGHT_MARGIN;

const Rectangle right_margin{
	.x = WINDOW_WIDTH_MARGIN_START,
	.y = 0,
	.width = WINDOW_WIDTH_MARGIN,
	.height = WINDOW_HEIGHT,
};

static uint score = 0;

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

void draw_next_tet(Tetramino tet) {
	DrawText("Next:", WINDOW_WIDTH_MARGIN_START + 8, 8, 20, WHITE);
	for (size_t i = 0; i < 4; ++i) {
		tet.blocks[i].draw_medium(
			WINDOW_WIDTH_MARGIN_START - 8, 20, -tet.get_x_offset(), -tet.get_y_offset()
		);
	}
}
void draw_hold_tet(std::optional<Tetramino> tet) {
	DrawText("Hold:", WINDOW_WIDTH_MARGIN_START + 8, 80, 20, WHITE);
	if (tet.has_value()) {
		for (size_t i = 0; i < 4; ++i) {
			tet.value().blocks[i].draw_medium(
				WINDOW_WIDTH_MARGIN_START - 8,
				92,
				-tet.value().get_x_offset(),
				-tet.value().get_y_offset()
			);
		}
	}
}

// returns true when window should close.
bool game() {
	uint64_t cycle_count = 0;
	int game_time = 0;
	int frames_per_fall = 40; // reduce this to increase speed and difficulty
	uint difficulty = 0;	  // reduce this to increase speed and difficulty
	vector<Block> blocks{};
	std::optional<Tetramino> hold_tet;
	Tetramino next_tet = create_random_tet();
	Tetramino tet = create_random_tet();
	Collision col{};
	int rotated_count = 0;

	// game loop
	while (!WindowShouldClose()) {
		vector<Block> total_blocks = blocks;
		total_blocks.insert(
			total_blocks.begin(), std::begin(tet.blocks), std::end(tet.blocks)
		);

		col = check_all_collisions(tet, blocks);
		if (IsKeyPressed(KEY_H) && !col.base.left) {
			tet.left();
		}
		if (IsKeyPressed(KEY_L) && !col.base.right) {
			tet.right();
		}
		if (IsKeyPressed(KEY_J) && !col.base.down) {
			tet.fall();
			game_time = 0;
		}
		if (IsKeyPressed(KEY_R)) {
			tet.rotate_ccw(blocks);
			if (rotated_count < 3) {
				game_time = game_time / 2;
			}

			++rotated_count;
		}

		// draw a ghost tetramino where it would land (draw happens below)
		auto ghost_tet = tet;
		while (!(check_collision(ghost_tet.blocks, blocks).down)) {
			ghost_tet.fall();
		}
		// instantly replace tetramino with ghost tetramino, (place it immediately)
		if (IsKeyPressed(KEY_SPACE)) {
			tet = ghost_tet;
			game_time = frames_per_fall - 1;
		}

		if (IsKeyPressed(KEY_S)) {
			auto temp = tet;

			if (hold_tet.has_value()) {
				tet.set_pattern(hold_tet->get_pattern(), hold_tet->blocks[0].color);
				hold_tet = temp;
			} else {
				tet = next_tet;
				hold_tet = temp;
				next_tet = create_random_tet();
			}
		}
		col = check_all_collisions(tet, blocks);

		if (game_time != 0 && game_time >= frames_per_fall) {
			game_time = 0;
			rotated_count = 0;
			++cycle_count;
			TraceLog(LOG_INFO, "cycle: %d", cycle_count);
			if (!col.base.down) {
				TraceLog(LOG_INFO, "Collision down");
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
						return false;
					}
				}

				blocks = total_blocks;
				tet = next_tet;
				next_tet = create_random_tet();

				difficulty = (score / 500);

				printf("difficulty:%d, score: %d\n", difficulty, score);
				frames_per_fall = std::max(5, 40 - (3 * static_cast<int>(difficulty)));
			}
		}

		// TraceLog(LOG_INFO, "frame: %d\n", game_time);
		BeginDrawing();
		ClearBackground(GRAY);
		DrawRectangleRec(right_margin, DARKGRAY);
		DrawText(
			std::format("level:\n{}", difficulty).c_str(),
			WINDOW_WIDTH_MARGIN_START + 4,
			WINDOW_HEIGHT_MARGIN_START - 24,
			16,
			WHITE
		);
		DrawText(
			std::format("score:\n{}", score).c_str(),
			WINDOW_WIDTH_MARGIN_START + 4,
			WINDOW_HEIGHT_MARGIN_START + 24,
			16,
			WHITE
		);
		draw_next_tet(next_tet);
		draw_hold_tet(hold_tet);
		// draw dotted line
		for (int i = 0; i < 16; i += 2) {
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

		draw_blocks(ghost_tet.blocks, 0, WINDOW_HEIGHT_MARGIN, 0.2F);

		draw_blocks(total_blocks, 0, WINDOW_HEIGHT_MARGIN);
		EndDrawing();
		++game_time;
	}
	return true;
}

int main() {
	// init
	SetTraceLogLevel(LOG_ALL);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);
	load_block_texture();

	game();

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_R)) {
			if (game()) {
				return 0;
			} else {
				TraceLog(LOG_INFO, "Restarted!");
			}
		}
		BeginDrawing();

		std::string score_str = std::format("score: {}", score);
		size_t loss_length = std::max(score_str.length(), static_cast<size_t>(11));
		size_t loss_width = (loss_length * 14);

		int text_x = static_cast<int>((WINDOW_WIDTH / 2) - (loss_width / 2));
		int text_y = (WINDOW_HEIGHT / 2) - 24;

		DrawRectangleRec(
			Rectangle{
				.x = static_cast<float>(text_x - 12),
				.y = static_cast<float>(text_y - 12),
				.width = static_cast<float>(loss_width),
				.height = (24 * 4) - 6
			},
			ColorAlpha(DARKGRAY, 0.3F)
		);
		DrawText("YOU LOSE.", text_x, text_y, 24, WHITE);
		DrawText(score_str.c_str(), text_x, text_y + 48, 24, WHITE);
		EndDrawing();
	}

	// close
	unload_block_texture();
	CloseWindow();

	return 0;
}
