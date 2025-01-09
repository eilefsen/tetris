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
static Texture2D mediumblock_texture;
static Texture2D tinyblock_texture;

void load_block_texture() {
	block_texture = LoadTexture("assets/block.png");
	mediumblock_texture = LoadTexture("assets/mediumblock.png");
	tinyblock_texture = LoadTexture("assets/tinyblock.png");
}
void unload_block_texture() {
	UnloadTexture(block_texture);
	UnloadTexture(mediumblock_texture);
	UnloadTexture(tinyblock_texture);
}

void Block::draw() { draw(0, 0); }
void Block::draw(int x_margin, int y_margin) {
	int x = (this->pos.x * BLOCK_SIZE) + x_margin;
	int y = (this->pos.y * BLOCK_SIZE) + y_margin;
	DrawTexture(block_texture, x, y, this->color);
}
void Block::draw_tiny() { draw_tiny(0, 0); }
void Block::draw_tiny(int x_margin, int y_margin) { draw_tiny(0, 0, x_margin, y_margin); }
void Block::draw_tiny(int x_offset, int y_offset, int x_margin, int y_margin) {
	int x = ((this->pos.x + x_offset) * TINYBLOCK_SIZE) + x_margin;
	int y = ((this->pos.y + y_offset) * TINYBLOCK_SIZE) + y_margin;
	DrawTexture(tinyblock_texture, x, y, this->color);
}

void Block::draw_medium() { draw_tiny(0, 0); }
void Block::draw_medium(int x_margin, int y_margin) {
	draw_tiny(0, 0, x_margin, y_margin);
}
void Block::draw_medium(int x_offset, int y_offset, int x_margin, int y_margin) {
	int x = ((this->pos.x + x_offset) * MEDIUMBLOCK_SIZE) + x_margin;
	int y = ((this->pos.y + y_offset) * MEDIUMBLOCK_SIZE) + y_margin;
	DrawTexture(mediumblock_texture, x, y, this->color);
}

void draw_blocks(std::vector<Block> blocks, int x_margin, int y_margin) {
	for (auto &b : blocks) {
		b.draw(x_margin, y_margin);
	}
}
