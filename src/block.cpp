#include <map>
#include <vector>

#include "block.hpp"

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

static Texture2D block_texture;

void load_block_texture() { block_texture = LoadTexture("assets/block.png"); }
void unload_block_texture() { UnloadTexture(block_texture); }

void Block::draw(int x_margin, int y_margin) {
	int x = (this->pos.x * BLOCK_SIZE) + x_margin;
	int y = (this->pos.y * BLOCK_SIZE) + y_margin;
	DrawTexture(block_texture, x, y, this->color);
}
void Block::draw() { draw(0, 0); }

void draw_blocks(std::vector<Block> blocks, int x_margin, int y_margin) {
	for (auto &b : blocks) {
		b.draw(x_margin, y_margin);
	}
}
