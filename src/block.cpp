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

Texture2D block_texture;
Texture2D mediumblock_texture;
Texture2D tinyblock_texture;

void load_block_texture() {
	block_texture = LoadTexture("data/block.png");
	mediumblock_texture = LoadTexture("data/mediumblock.png");
	tinyblock_texture = LoadTexture("data/tinyblock.png");
}
void unload_block_texture() {
	UnloadTexture(block_texture);
	UnloadTexture(mediumblock_texture);
	UnloadTexture(tinyblock_texture);
}

void Block::draw(int x_margin, int y_margin, int x_offset, int y_offset, float opacity) {
	return this->draw_pro(x_margin, y_margin, x_offset, y_offset, block_texture, opacity);
}

void Block::draw_medium(
	int x_margin, int y_margin, int x_offset, int y_offset, float opacity
) {
	return this->draw_pro(
		x_margin, y_margin, x_offset, y_offset, mediumblock_texture, opacity
	);
}

void Block::draw_tiny(
	int x_margin, int y_margin, int x_offset, int y_offset, float opacity
) {
	return this->draw_pro(
		x_margin, y_margin, x_offset, y_offset, tinyblock_texture, opacity
	);
}

void Block::draw_pro(
	int x_margin, int y_margin, int x_offset, int y_offset, Texture2D texture,
	float opacity
) {
	int x = ((this->pos.x + x_offset) * texture.width) + x_margin;
	int y = ((this->pos.y + y_offset) * texture.height) + y_margin;
	DrawTexture(texture, x, y, ColorAlpha(this->color, opacity));
}

void draw_blocks(std::vector<Block> blocks, int x_margin, int y_margin, float opacity) {
	for (auto &b : blocks) {
		b.draw(x_margin, y_margin, 0, 0, opacity);
	}
}

void draw_blocks(std::array<Block, 4> blocks, int x_margin, int y_margin, float opacity) {
	for (auto &b : blocks) {
		b.draw(x_margin, y_margin, 0, 0, opacity);
	}
}
