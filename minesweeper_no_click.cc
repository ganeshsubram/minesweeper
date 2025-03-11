//
// Copyright © 2025 Glydways, Inc.
// https://glydways.com/
//

#include <array>
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

using CellValue = std::int8_t;
using Position = std::pair<size_t, size_t>;

struct Cell {
  static constexpr CellValue BOMB = -1;
  static constexpr CellValue SPACE = 0;

  CellValue value{};
  bool is_revealed{};

  std::string to_string(bool force_reveal = false) const {
    if (!force_reveal && !is_revealed) {
      return "-";
    }

    if (value == BOMB) {
      return "X";
    }

    if (value == SPACE) {
      return " ";
    }

    return std::to_string(value);
  }
};

enum class ClickResult {
  IN_PROGRESS,
  INVALID_MOVE,
  WIN,
  LOSE,
};

template <size_t NumRowsT, size_t NumColsT> class Minesweeper {
public:
  Minesweeper(const std::vector<Position> &bomb_positions) {
    insert_bombs(bomb_positions);
  }

  ClickResult click(const Position &click) { return ClickResult::IN_PROGRESS; }

  void print_board() { print_board_internal(false); }

  void print_board_debug() { print_board_internal(true); }

private:
  void insert_bombs(const std::vector<Position> &bomb_positions) {
    //
    // Check bomb locations are legit
    //
    assert(bomb_positions.size() < NumRowsT * NumColsT);
    for (const auto &[row, col] : bomb_positions) {
      assert(row < NumRowsT);
      assert(col < NumColsT);
    }

    //
    // Insert bombs
    //
    for (const auto &[row, col] : bomb_positions) {
      board_[row][col].value = Cell::BOMB;
    }
  }

  void print_board_internal(bool show_all_cells) const {
    std::cout << std::endl;
    for (const auto &col : board_) {
      for (const Cell &cell : col) {
        std::cout << cell.to_string(show_all_cells) << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  std::array<std::array<Cell, NumRowsT>, NumColsT> board_;
};

int main() {

  std::vector<Position> bomb_locations = {
      {1, 3},
      {2, 2},
  };
  std::vector<Position> moves = {{2, 4}, {0, 1}, {3, 2}, {2, 2}};

  Minesweeper<5, 7> game{bomb_locations};
  game.print_board_debug();

  for (const Position &move : moves) {
    std::cout << "Move: [" << move.first << ", " << move.second << "]"
              << std::endl;

    ClickResult result = game.click(move);
    game.print_board_debug();

    if (result == ClickResult::WIN) {
      std::cout << "Result: Win!" << std::endl;
      break;
    } else if (result == ClickResult::LOSE) {
      std::cout << "Result: Lose :(" << std::endl;
      break;
    }
  }

  return 0;
}
