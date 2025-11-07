#pragma once
// #include <unordered_map>
// #include <map>
// #include <algorithm>
// #include <numeric>
#include <array>
#include <functional>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include <userver/formats/json.hpp>

namespace ScrabbleGame {

struct Tile {
  char32_t letter;
  int points;
};

} // namespace ScrabbleGame

constexpr std::array<ScrabbleGame::Tile, 128> defaultTiles{
    {{U'А', 1}, {U'А', 1}, {U'А', 1},  {U'А', 1},  {U'А', 1},  {U'А', 1},
     {U'А', 1}, {U'А', 1}, {U'А', 1},  {U'А', 1},  {U'Б', 3},  {U'Б', 3},
     {U'Б', 3}, {U'В', 2}, {U'В', 2},  {U'В', 2},  {U'В', 2},  {U'В', 2},
     {U'Г', 3}, {U'Г', 3}, {U'Г', 3},  {U'Д', 2},  {U'Д', 2},  {U'Д', 2},
     {U'Д', 2}, {U'Д', 2}, {U'Е', 1},  {U'Е', 1},  {U'Е', 1},  {U'Е', 1},
     {U'Е', 1}, {U'Е', 1}, {U'Е', 1},  {U'Е', 1},  {U'Е', 1},  {U'Ж', 5},
     {U'Ж', 5}, {U'З', 5}, {U'З', 5},  {U'И', 1},  {U'И', 1},  {U'И', 1},
     {U'И', 1}, {U'И', 1}, {U'И', 1},  {U'И', 1},  {U'И', 1},  {U'Й', 2},
     {U'Й', 2}, {U'Й', 2}, {U'Й', 2},  {U'К', 2},  {U'К', 2},  {U'К', 2},
     {U'К', 2}, {U'К', 2}, {U'К', 2},  {U'Л', 2},  {U'Л', 2},  {U'Л', 2},
     {U'Л', 2}, {U'М', 2}, {U'М', 2},  {U'М', 2},  {U'М', 2},  {U'М', 2},
     {U'Н', 1}, {U'Н', 1}, {U'Н', 1},  {U'Н', 1},  {U'Н', 1},  {U'Н', 1},
     {U'Н', 1}, {U'Н', 1}, {U'О', 1},  {U'О', 1},  {U'О', 1},  {U'О', 1},
     {U'О', 1}, {U'О', 1}, {U'О', 1},  {U'О', 1},  {U'О', 1},  {U'О', 1},
     {U'П', 2}, {U'П', 2}, {U'П', 2},  {U'П', 2},  {U'П', 2},  {U'П', 2},
     {U'Р', 2}, {U'Р', 2}, {U'Р', 2},  {U'Р', 2},  {U'Р', 2},  {U'Р', 2},
     {U'С', 2}, {U'С', 2}, {U'С', 2},  {U'С', 2},  {U'С', 2},  {U'С', 2},
     {U'Т', 2}, {U'Т', 2}, {U'Т', 2},  {U'Т', 2},  {U'Т', 2},  {U'У', 3},
     {U'У', 3}, {U'У', 3}, {U'Ф', 10}, {U'Х', 5},  {U'Х', 5},  {U'Ц', 10},
     {U'Ч', 5}, {U'Ч', 5}, {U'Ш', 10}, {U'Щ', 10}, {U'Ъ', 10}, {U'Ы', 5},
     {U'Ы', 5}, {U'Ь', 5}, {U'Ь', 5},  {U'Э', 10}, {U'Ю', 10}, {U'Я', 3},
     {U'Я', 3}, {U'Я', 3}}};

namespace ScrabbleGame {

struct PlayerState {
  std::vector<Tile> hand;
  int score = 0;
};

class Randomizer {
  const int bag_size;
  const int jokers_num;
  int current_max;

  std::mt19937 mt_random;

public:
  Randomizer(const int &bag_size, const int &jokers_num);

  /*
   * @brief returns random number [0, bag_size)
   */
  int random_tile();
  /*
   * @brief return random number [0, bag_size-jokers_num)
   */
  int random_tile_wo_jokers();
  /*
   * @brief returns random number [0, bag_size-num_of_returns)
   *
   * @retval {int} -1 if num_of_returns >= bag_size
   */
  int decrement_random_tile();
};

struct GameState {
public:
  // max num of tiles in player hand
  const int TILES_MAX_IN_HAND;

  // coordinates of new_tiles in same order
  std::vector<std::vector<int>> new_tiles_coordinates;
  // new_tiles that player tries to place
  std::vector<Tile> new_tiles;
  /*
   * @brief score for new tiles
   *
   * @param {-1} new_tiles placement is bad
   * @param {>0} new_tiles placement is good, and score for them is score
   */
  int score = -1;

  std::vector<PlayerState> playersState;
  int currentPlayer; // index in playersState
  std::vector<Tile> bag;
  /*
   * @brief holds two-dimensional vector, which represents board
   *
   * @notes boards[x][y]
   */
  std::vector<std::vector<std::optional<Tile>>> board;

  /*
   * @brief fills player's hand with tiles
   *
   * @param {index} player index fron vector playersState
   */
  void FillHand(const int index);

  /*
   * @brief constructor of GameState
   *
   * @param {tiles_max} max num of tiles in player hand
   * @param {players_num} number of players
   * @param {bag_size} number of tiles in bag
   * @param {jokers_num} number of jokers in bag
   * @param {default_tiles} array with all possible tiles with tiles necessary
   * frequency
   */
  GameState(const int &tiles_max, const int &players_num, const int &bag_size,
            const int &jokers_num, const std::array<Tile, 128> &default_tiles);

private:
  Randomizer randomizer;
  /*
   * @brief fills players bags with tiles, used in initializer
   */
  void FillBag_(const int &bag_size, const int &jokers_num,
                const std::array<Tile, 128> &default_tiles);
  /*
   * @brief Tries to draw a Tile from bag
   * @retval {Tile} returns tile from back of bag vector, if value of Tile ==
   * -1, then bag is empty
   */
  Tile DrawTile_();
};

class ScrabbleGame {
public:
  /*
   * @brief Creates new game instance with default values
   *
   * @param {tiles_max} max num of tiles in player hand, def=7, recomended >= 7
   * @param {players_num} num of players, def=2, must be >= 2
   * @param {bag_size} how much letters in bag
   * @param {jokers_num} how much jokers in bag
   * @param {default_tiles} array with all possible tiles with tiles necessary
   * frequency
   */
  ScrabbleGame(std::function<bool(const std::u32string &word)> word_checker,
               const int &tiles_max = 7, const int &players_num = 2,
               const int &bag_size = 131, const int &jokers_num = 3,
               const std::array<Tile, 128> &default_tiles = defaultTiles);

  /*
   * @brief Tries to place a word on board
   *
   * @param {coordinates} vector{{x,y}, ...} where Tiles placed
   * @param {word} vector{Tile, ...} in order exactly like in coordinates
   * @param {word_checker(std::u32string&)} function to check existence of
   * words, should return true if word exists, else if not
   *
   * @retval {state_.score} state_.score becomes 0 or more if tiles placement is
   * correct
   *
   * @throws {char*} error
   */
  int TryPlaceTiles(std::vector<std::vector<int>> &coordinates,
                    std::vector<Tile> &tiles);

  /*
   * @brief Places tiles on board if possible
   *
   * @retval {state_.score>=0} score for tiles, if tiles is placed
   * @retval {state_.score==-1} if tiles can't be placed
   */
  int SubmitWord();

private:
  GameState state_;
  std::function<bool(const std::u32string &word)> word_checker;

  /*
   * @brief Checks placement of tiles inside TryPlaceTiles()
   *
   * @param {word_checker(std::u32string&)} function to check existence of
   * words, should return true if word exists, else if not
   *
   * @retval {state_.score} state_.score becomes 0 or more if tiles placement is
   * correct
   *
   * @throws {char*} error
   */
  void TilesCheck();

  // TODO: receives only horiz or vert placed tiles,
  // TODO: can return words with one letter
  /*
   * @brief gathers all words thah were formed by placed tiles
   *
   * @retval {vector<u32string>} vector with all words
   *
   *
   * @param {&new_board} board with all new placed tiles
   * @param {&coordinates} vector with coordinates of all new placed tiles
   * @param {&horizontal} should be 1 if all tiles in horizontal row, 0 if all
   * tiles in vertical row
   */
  std::vector<std::u32string>
  GetNewWords_(std::vector<std::vector<std::optional<Tile>>> &new_board,
               std::vector<std::vector<int>> &coordinates,
               const bool &horizontal);

  /*
   * @brief gathers word that were formed by one tile, which belongs to tiles
   * forming horizontal line
   *
   * @retval {u32string} word formed by this tile
   *
   * @param {&new_board} board with all new placed tiles
   * @param {&tile_coords} coords of tile to check formed words from
   */
  std::u32string
  horizontal_check_(std::vector<std::vector<std::optional<Tile>>> &new_board,
                    std::vector<int> &tile_coords);

  /*
   * @brief gathers word that were formed by one tile, which belongs to tiles
   * forming vertical line
   *
   * @retval {u32string} word formed by this tile
   *
   * @param {&new_board} board with all new placed tiles
   * @param {&tile_coords} coords of tile to check formed words from
   */
  std::u32string
  vertical_check_(std::vector<std::vector<std::optional<Tile>>> &new_board,
                  std::vector<int> &tile_coords);

  /*
   * @brief calculates value of word
   *
   * @param {&word} letter sequence to score, joker = '*'
   *
   * @retval {int} value of word
   */
  int calculate_score_(const std::u32string &word);
};

} // namespace ScrabbleGame
