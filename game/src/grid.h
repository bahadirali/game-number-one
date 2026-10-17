#pragma once

#include <cstddef>
#include <vector>

namespace game {

struct Cell {
    int x = 0;
    int y = 0;

    friend bool operator==(Cell, Cell) = default;
};

enum class Tile {
    Floor,
    Wall,
};

// A rectangular grid of tiles. Cells outside the grid count as walls.
class Grid {
public:
    Grid() = default;
    Grid(int width, int height, Tile fill = Tile::Floor)
        : width_(width), height_(height),
          tiles_(static_cast<std::size_t>(width * height), fill) {}

    int width() const { return width_; }
    int height() const { return height_; }

    bool contains(Cell cell) const {
        return cell.x >= 0 && cell.y >= 0 && cell.x < width_ && cell.y < height_;
    }

    Tile at(Cell cell) const { return contains(cell) ? tiles_[index(cell)] : Tile::Wall; }

    void set(Cell cell, Tile tile) {
        if (contains(cell)) {
            tiles_[index(cell)] = tile;
        }
    }

private:
    std::size_t index(Cell cell) const { return static_cast<std::size_t>(cell.y * width_ + cell.x); }

    int width_ = 0;
    int height_ = 0;
    std::vector<Tile> tiles_;
};

} // namespace game
