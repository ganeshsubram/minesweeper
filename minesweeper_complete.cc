#include <iostream>
#include <cstdint>
#include <array>
#include <utility>
#include <functional>
#include <vector>
#include <queue>
#include <assert.h>

using CellValue = std::int8_t;
using Position = std::pair<size_t, size_t>;

struct Cell {
  static constexpr CellValue BOMB = -1;
  static constexpr CellValue SPACE = 0;

  CellValue value;
  bool is_revealed;

  std::string to_string(bool apply_revealed = true) const {
    if (apply_revealed && !is_revealed) {
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

template<size_t NumRowsT, size_t NumColsT>
class Minesweeper {
public:
  Minesweeper(const std::vector<Position> &bomb_positions) {
      insert_bombs(bomb_positions);
    }

  ClickResult click(const Position& click) {
    const auto&[row, col] = click;

    // Check the cell exists in the board
    if (row > NumRowsT || col > NumColsT) {
      return ClickResult::INVALID_MOVE;
    }

    // Check if the cell has already been clicked/revealed.
    if (board_[row][col].is_revealed) {
      return ClickResult::IN_PROGRESS;
    }

    // Reveal the cell
    board_[row][col].is_revealed = true;

    // Check if it was a bomb - end the game.
    if (board_[row][col].value == Cell::BOMB){
      return ClickResult::LOSE;
    }
    
    //
    // Clicked a valid cell - reveal the remaining cells.
    //
    std::queue<Position> visit_positions;
    visit_positions.push(click);
    do {
      Position current_pos = visit_positions.front();
      visit_positions.pop();
      Cell& current_cell = board_[current_pos.first][current_pos.second];

      if (current_cell.value == Cell::BOMB) {
        continue;
      }

      //
      // Cell is a number - reveal
      //
      current_cell.is_revealed = true;

      //
      // If it's a space, we need to check the surrounding cells
      //
      if (current_cell.value == Cell::SPACE) {
        visit_surrounding_cells(current_pos, [&visit_positions](Cell& cell, const Position& position){
            if (cell.value != Cell::BOMB && !cell.is_revealed) {
              visit_positions.push(position);
            }
        });
      }
    } while (!visit_positions.empty());

    //
    // TODO - evaluate if all non-bomb cells are revelaed for win
    //

    return ClickResult::IN_PROGRESS;
  }

  void print_board() const {
    std::cout << std::endl;
    for (const auto& col : board_) {
      for(const Cell& cell : col) {
        std::cout << cell.to_string() << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

private:
  void insert_bombs(const std::vector<Position> &bomb_positions) {
      //
      // Check bomb locations are legit
      //
      assert(bomb_positions.size() < NumRowsT * NumColsT);
      for(const auto& [row, col] : bomb_positions) {
        assert(row < NumRowsT);
        assert(col < NumColsT);
      }
      // TODO(ganesh) - check bomb positions are unique

      //
      // Insert bombs
      //
      for (const auto& [row, col] : bomb_positions) {
        board_[row][col].value = Cell::BOMB;

        //
        // Visit surrounding cells and incremment bomb count
        //
        visit_surrounding_cells({row, col}, [](Cell& cell, Position& /*position*/){
            if(cell.value != Cell::BOMB) {
              cell.value++;
            }
        });
      }
  }

  using VisitCallback = std::function<void(Cell&, Position&)>;
  void visit_surrounding_cells(Position cell, VisitCallback callback) {
     const auto& [row, col] = cell;
     for (const auto& [dx, dy] : SURROUNDING_DX_DY) {
          //
          // Bounds check
          //
          if (row == 0 && dx < 0) {
            continue;
          } 
          if(col == 0 && dy < 0) {
            continue;
          }
          if (row == (NumRowsT - 1) && dx > 0) {
            continue;
          } 
          if(col == (NumColsT - 1) && dy > 0) {
            continue;
          }

          const auto new_row = row + dx;
          const auto new_col = col + dy;
          Position cell_position{ new_row, new_col};
          callback(board_[new_row][new_col], cell_position);
     }
  }

  static constexpr std::array<std::pair<std::int8_t, std::int8_t>, 8> SURROUNDING_DX_DY {
      std::pair{-1, -1}, 
      std::pair{-1, 0},
      std::pair{0, -1},
      std::pair{1, 1},
      std::pair{1, 0},
      std::pair{0, 1},
      std::pair{-1, 1},
      std::pair{1, -1},
    };
  
  std::array<std::array<Cell, NumRowsT>, NumColsT> board_;
};


int main() {

  std::vector<Position> bomb_locations = {
    {1, 3}, {2, 2},
  };
  std::vector<Position> moves = {
    {2, 4}, {0, 1}, {3, 2}, {2, 2}
  };

  Minesweeper<5, 7> game{bomb_locations};
  game.print_board();

  for(const Position& move : moves) {
    std::cout << "Move: [" << move.first << ", " << move.second << "]" << std::endl;

    ClickResult result = game.click(move);
    game.print_board();

    if (result == ClickResult::WIN) {
      std::cout << "Result: Win!" << std::endl;
      break;
    }
    else if (result == ClickResult::LOSE) {
      std::cout << "Result: Lose :(" << std::endl;
      break;
    }
  }

  return 0;
}

