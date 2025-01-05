#include "collision.hpp"
#include "tet.hpp"

Collision check_all_collisions(Tetramino tet, std::vector<Block> board) {
	auto base = check_collision(tet.blocks, board);
	auto rotated = check_collision(tet.blocks, board);

	return Collision{
		.base = base,
		.rotated = rotated,
	};
}

CollisionBase check_collision(std::array<Block, 4> blocks, std::vector<Block> board) {
	CollisionBase col;
	int lowest_y = 0;
	int rightest_x = 0;
	int leftest_x = GRID_HEIGHT - 1;

	for (int i = 0; i < 4; ++i) {
		Coordinate t = blocks[i].pos;

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

CollisionBase check_obstruction(std::array<Block, 4> blocks, std::vector<Block> board) {
	CollisionBase col;
	int lowest_y = 0;
	int rightest_x = 0;
	int leftest_x = GRID_HEIGHT - 1;

	for (int i = 0; i < 4; ++i) {
		Coordinate t = blocks[i].pos;

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
			if (xMatch && t.y == b.pos.y) {
				col.down = true;
			}
			if (xMatch && t.y == b.pos.y) {
				col.up = true;
			}
			bool yMatch = (t.y == b.pos.y);
			if (yMatch && t.x == b.pos.x) {
				col.right = true;
			}
			if (yMatch && t.x == b.pos.x) {
				col.left = true;
			}
		}
	}

	if (lowest_y >= GRID_HEIGHT) {
		col.down = true;
	}
	if (rightest_x >= GRID_WIDTH) {
		col.right = true;
	}
	if (leftest_x <= -1) {
		col.left = true;
	}

	return col;
}
