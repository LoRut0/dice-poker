#include "ScrabbleGame.hpp"
#include <iostream>
#include <string>

bool word_checker(const std::u32string &word) {
  if (word.size() > 2)
    return 1;
  return 0;
};

void draw(const ScrabbleGame::GameState state) {
  const auto &board = state.board;
  std::cout << ' ';
  for (int x = 0; x < board.size(); ++x) {
    for (int y = 0; y < board[0].size(); ++y) {
      if (!board[x][y]) {
        std::cout << "* ";
      }
    }
    std::cout << '\n';
  }
}

int main() {
  ScrabbleGame::ScrabbleGame game(word_checker);
  std::vector<std::vector<int>> coords{{0, 0}, {0, 1}, {0, 2}};
  std::vector<ScrabbleGame::Tile> tiles{{U'К', 2}, {U'О', 2}, {U'Т', 2}};
  game.TryPlaceTiles(coords, tiles);

  return 0;
}
