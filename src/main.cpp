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
	Coordinate orig1;
	Coordinate orig2;
	Coordinate orig3;
	Coordinate orig4;
	int fall_count = 0;
	int right_count = 0;
	bool rotated = false;

  public:
	Block one;
	Block two;
	Block three;
	Block four;

	void fall() {
		one.pos.y += 1;
		two.pos.y += 1;
		three.pos.y += 1;
		four.pos.y += 1;
		++fall_count;
	}
	void left() {
		one.pos.x -= 1;
		two.pos.x -= 1;
		three.pos.x -= 1;
		four.pos.x -= 1;
		--right_count;
	}
	void right() {
		one.pos.x += 1;
		two.pos.x += 1;
		three.pos.x += 1;
		four.pos.x += 1;
		++right_count;
	}

	void rotate() {
		int x1 = orig1.x;
		int x2 = orig2.x;
		int x3 = orig3.x;
		int x4 = orig4.x;

		if (!rotated) {
			orig1.x = orig1.y;
			orig2.x = orig2.y;
			orig3.x = orig3.y;
			orig4.x = orig4.y;
			orig1.y = -x1;
			orig2.y = -x2;
			orig3.y = -x3;
			orig4.y = -x4;
			rotated = true;
		} else {
			orig1.x = -orig1.y;
			orig2.x = -orig2.y;
			orig3.x = -orig3.y;
			orig4.x = -orig4.y;
			orig1.y = x1;
			orig2.y = x2;
			orig3.y = x3;
			orig4.y = x4;
			rotated = false;
		}

		one.pos.x = orig1.x + right_count;
		two.pos.x = orig2.x + right_count;
		three.pos.x = orig3.x + right_count;
		four.pos.x = orig4.x + right_count;

		one.pos.y = orig1.y + fall_count;
		two.pos.y = orig2.y + fall_count;
		three.pos.y = orig3.y + fall_count;
		four.pos.y = orig4.y + fall_count;
	}

	int lowest_y() {
		int a = std::max(one.pos.y, two.pos.y);
		int b = std::max(three.pos.y, four.pos.y);
		return std::max(a, b);
	}

	Tetramino *clone() const { return new Tetramino(*this); }

	Tetramino(
		Color color, Coordinate pos1, Coordinate pos2, Coordinate pos3, Coordinate pos4
	)
		: one(Block{pos1, .color = color}), two(Block{pos2, .color = color}),
		  three(Block{pos3, .color = color}), four(Block{pos4, .color = color}),
		  orig1(pos1), orig2(pos2), orig3(pos3), orig4(pos4) {}
};

static const Tetramino *I_TET = new Tetramino(
	SKYBLUE,
	{
		.x = 0,
		.y = 0,
	},
	{
		.x = 1,
		.y = 0,
	},
	{
		.x = 2,
		.y = 0,
	},
	{
		.x = 3,
		.y = 0,
	}
);
static const Tetramino *T_TET = new Tetramino(
	PURPLE,
	{
		.x = 0,
		.y = 0,
	},
	{
		.x = 1,
		.y = 0,
	},
	{
		.x = 2,
		.y = 0,
	},
	{
		.x = 1,
		.y = 1,
	}
);

void move(Tetramino *t) {
	// TODO: add key hold
	if (IsKeyPressed(KEY_H)) {
		t->left();
	}
	// TODO: add key hold
	if (IsKeyPressed(KEY_L)) {
		t->right();
	}
	if (IsKeyPressed(KEY_J)) {
		t->fall();
	}
	if (IsKeyPressed(KEY_R)) {
		t->rotate();
	}
}

void draw_blocks(std::vector<Block> blocks);

int main() {
	// init
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris!");
	SetTargetFPS(FPS_TARGET);

	std::vector<Block> blocks{};
	Tetramino *tet = I_TET->clone();

	int game_time = 0;

	// run
	while (!WindowShouldClose()) {
		// update
		std::vector<Block> total_blocks = blocks;
		total_blocks.push_back(tet->one);
		total_blocks.push_back(tet->two);
		total_blocks.push_back(tet->three);
		total_blocks.push_back(tet->four);

		if (game_time % FPS_TARGET == 0) {
			tet->fall();
		}
		move(tet);
		if (tet->lowest_y() == GRID_HEIGHT) {
			blocks = total_blocks;
			tet = T_TET->clone();
		}

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
