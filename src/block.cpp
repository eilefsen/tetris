#include <map>
#include <vector>

#include "block.hpp"

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

void draw_blocks(std::vector<Block> blocks) {
	for (const auto &b : blocks) {
		float x = b.pos.x * BLOCK_SIZE;
		float y = b.pos.y * BLOCK_SIZE;
		DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, b.color);
	}
}
