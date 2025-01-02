#include <algorithm>
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
	Coordinate orig[4];
	int pattern[4][4];

  public:
	Block blocks[4];

	void fall() {
		for (auto &b : blocks) {
			b.pos.y += 1;
		}
	}
	void left() {
		for (auto &b : blocks) {
			b.pos.x -= 1;
		}
	}
	void right() {
		for (auto &b : blocks) {
			b.pos.x += 1;
		}
	}

	void rotate() {
		for (int i = 0; i < 4; ++i) {
			int x_offset = blocks[i].pos.x - orig[i].x;
			int y_offset = blocks[i].pos.y - orig[i].y;

			// TODO: Refine rotation
			Coordinate new_pos{.x = orig[i].y + 1, .y = -orig[i].x};

			blocks[i].pos.x = new_pos.x + x_offset;
			blocks[i].pos.y = new_pos.y + y_offset;
			orig[i].x = new_pos.x;
			orig[i].y = new_pos.y;
		}
	}

	Tetramino clone() const { return Tetramino(*this); }

	Tetramino(Color color, Coordinate positions[4])
		: blocks{Block{positions[0], .color = color}, Block{positions[1], .color = color}, Block{positions[2], .color = color}, Block{positions[3], .color = color}},
		  orig{positions[0], positions[1], positions[2], positions[3]} {}
};

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

	if (lowest_y == GRID_HEIGHT - 1) {
		col.down = true;
	}
	if (rightest_x == GRID_WIDTH - 1) {
		col.right = true;
	}
	if (leftest_x == 0) {
		col.left = true;
	}

	return col;
}

Tetramino create_i_tet() {
	Coordinate pos[4]{
		{.x = 0, .y = 0},
		{.x = 1, .y = 0},
		{.x = 2, .y = 0},
		{.x = 3, .y = 0},
	};
	return Tetramino(SKYBLUE, pos);
}

Tetramino create_t_tet() {
	Coordinate pos[4]{
		{.x = 0, .y = 0},
		{.x = 1, .y = 0},
		{.x = 2, .y = 0},
		{.x = 1, .y = 1},
	};
	return Tetramino(PURPLE, pos);
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

int main() {
	// init
	SetTraceLogLevel(LOG_ALL);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);

	std::vector<Block> blocks{};
	Tetramino tet = create_i_tet();

	int game_time = 0;
	Collision col{};

	// run
	while (!WindowShouldClose()) {
		// update
		std::vector<Block> total_blocks = blocks;
		total_blocks.insert(
			total_blocks.begin(), std::begin(tet.blocks), std::end(tet.blocks)
		);

		if (game_time % FPS_TARGET == 0) {
			tet.fall();
			game_time = 0;
		}

		printf("%d%d%d\n", col.left, col.down, col.right);
		if (col.down) {
			blocks = total_blocks;
			tet = create_t_tet();
		} else {
			move(&tet, col);
		}
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
