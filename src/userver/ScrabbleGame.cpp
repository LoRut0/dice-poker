#include "ScrabbleGame.hpp"

namespace ScrabbleGame {

Randomizer::Randomizer(const int& bag_size, const int& jokers_num) : bag_size(bag_size), jokers_num(jokers_num), current_max(bag_size)
{
    std::random_device rand_device;
    mt_random.seed(rand_device());
}

int Randomizer::random_tile() {
    std::uniform_int_distribution distribution(0, bag_size-1);
    return distribution(mt_random);
}

int Randomizer::random_tile_wo_jokers() {
    std::uniform_int_distribution distribution(0, bag_size-1-jokers_num);
    return distribution(mt_random);
}

int Randomizer::decrement_random_tile() {
    if (current_max <= 0) return -1;
    current_max--;
    std::uniform_int_distribution distribution(0, current_max);
    return distribution(mt_random);
}

ScrabbleGame::ScrabbleGame(
    std::function<bool(const std::u32string &word)> word_checker,
    const int& tiles_max,
    const int& players_num,
    const int& bag_size,
    const int& jokers_num,
    const std::array<Tile, 128>& default_tiles) :
        state_(tiles_max, players_num, bag_size, jokers_num, default_tiles), word_checker(word_checker) {};

GameState::GameState(
    const int& tiles_max,
    const int& players_num,
    const int& bag_size,
    const int& jokers_num,
    const std::array<Tile, 128>& default_tiles
) : TILES_MAX_IN_HAND(tiles_max), randomizer(bag_size, jokers_num)
{
    FillBag_(bag_size, jokers_num, default_tiles);
    //TODO: dimensions of board must be settable
    board.resize(15);
    for (auto& column : board) {
        column.resize(15);
    }
}

void GameState::FillBag_(const int& bag_size, const int& jokers_num, const std::array<Tile, 128>& default_tiles) {
    if (bag_size == 131 and jokers_num == 3) {
        for (int i = 0; i < 127; ++i) {
            bag[i] = default_tiles[i];
        }
        for (int i = 0; i < jokers_num; ++i) {
            bag[128 + i] = { '*', 0 };
        }
        return;
    }
    
    for (int i = 0; i < bag_size; ++i) {
        bag[i] = default_tiles[ randomizer.random_tile_wo_jokers() ];
    }
    for (int i = 0; i < jokers_num; ++i) {
        bag[ bag_size + i ] = { '*', 0};
    }
    return;
}

void GameState::FillHand(const int index) {
    PlayerState current_player =  playersState[index];
    
    while (current_player.hand.size() < TILES_MAX_IN_HAND) {
        Tile new_tile = DrawTile_();
        if (new_tile.letter == 0) return;
        current_player.hand.push_back(new_tile);
    }
    return;
}

Tile GameState::DrawTile_() {
    if (bag.size() == 0) return {0, -1};
    
    int rand_int = randomizer.decrement_random_tile();
    Tile return_tile = bag[rand_int];
    bag[rand_int] = bag[bag.size() - 1];
    bag.pop_back();    
    
    return return_tile;
}

void ScrabbleGame::TilesCheck()
{
    auto& coordinates = state_.new_tiles_coordinates;
    auto& tiles = state_.new_tiles;
    auto new_board = state_.board;
    
    //to redo from here
    bool horizontal = 1;
    bool vertical = 1;
    std::vector<int> last_coords = coordinates[0];
    for (int i = 1; i < coordinates.size(); i++) {
        const auto& tile_coords = coordinates[i];
        if (tile_coords[0] != last_coords[0]) vertical = 0;
    }
    last_coords = coordinates[0];
    for (int i = 1; i < coordinates.size(); i++) {
        const auto& tile_coords = coordinates[i];
        if (tile_coords[1] != last_coords[1]) horizontal = 0;
    }
    
    if(!vertical && !horizontal) throw "Tiles are not in line";
    //to here

    for (int i = 0; i < coordinates.size(); i++) {
        const auto& tile_coords = coordinates[i];

        if (new_board[ tile_coords[0] ] [ tile_coords[1] ])
            throw "Coordinates already occupied by another Tile";
        
        new_board[ tile_coords[0] ] [ tile_coords[1] ] = tiles[i];
    }
    
    std::vector<std::u32string> words = GetNewWords_(new_board, coordinates, horizontal);
    
    for (const auto& word : words) {
        if (!word_checker(word)) {
            state_.score = -1;
            return;
        }
        state_.score += calculate_score_(word);
    }

    return;
}

int ScrabbleGame::TryPlaceTiles(
    std::vector<std::vector<int>>& coordinates,
    std::vector<Tile>& tiles
    )
{
    state_.score = -1;

    if (coordinates.size() != tiles.size()) throw "Vector sizes does not match"; 

    state_.new_tiles_coordinates = std::move(coordinates);
    state_.new_tiles = std::move(tiles);

    TilesCheck();
    return state_.score;
}

int ScrabbleGame::SubmitWord()
{
    if (state_.score == -1) return -1;

    auto& new_tiles = state_.new_tiles;
    auto& new_tiles_coordinates = state_.new_tiles_coordinates;

    for (int i = 0; i < new_tiles.size(); ++i) {
        const int& tile_x = new_tiles_coordinates[i][0];
        const int& tile_y = new_tiles_coordinates[i][1];
        state_.board[tile_x][tile_y] = new_tiles[i];
    }
    return state_.score;
}

std::vector<std::u32string> ScrabbleGame::GetNewWords_(std::vector<std::vector<std::optional<Tile>>> &new_board, std::vector<std::vector<int>> &coordinates, const bool &horizontal) 
{
    //TODO: may be better to have possibility to place tiles not in a row
    std::vector<std::u32string> words;
    if (horizontal) {
        //horizontal check
        {
            words.push_back( horizontal_check_(new_board, coordinates[0]) );
        }
        //vertical checks
        for (int i = 0; i < coordinates.size(); i++) {
            words.push_back( vertical_check_(new_board, coordinates[i]) );
        }
    }
    else 
    {
        //horizontal check
        for (int i = 0; i < coordinates.size(); i++) {
            words.push_back( horizontal_check_(new_board, coordinates[i]) );
        }
        //vertical checks
        {
            words.push_back( vertical_check_(new_board, coordinates[0]) );
        }
    }
    return words;
}

std::u32string ScrabbleGame::horizontal_check_(std::vector<std::vector<std::optional<Tile>>> &new_board, std::vector<int> &tile_coords) {
    std::u32string word;
    const int& tile_x = tile_coords[0];
    const int& tile_y = tile_coords[1];
    
    for (int x = tile_x - 1; x >= 0; x--) {
        if ( !new_board[x][tile_y] ) break;
        word += new_board[x][tile_y]->letter;
    }
    word = std::u32string(word.rbegin(), word.rend()) + new_board[tile_x][tile_y]->letter;
    for (int x = tile_x + 1; x < new_board.size(); x++) {
        if (!new_board[x][tile_y]) break;
        word += new_board[x][tile_y]->letter;
    }

    return word;
}

std::u32string ScrabbleGame::vertical_check_(std::vector<std::vector<std::optional<Tile>>> &new_board, std::vector<int> &tile_coords) {
    std::u32string word;
    const int& tile_x = tile_coords[0];
    const int& tile_y = tile_coords[1];
    
    for (int y = tile_y - 1; y >= 0; y--) {
        if (!new_board[tile_x][y]) break;
        word += new_board[tile_x][y]->letter;
    }
    word = std::u32string(word.rbegin(), word.rend()) + new_board[tile_x][tile_y]->letter;
    for (int y = tile_y + 1; y < new_board[0].size(); y++) {
        if (!new_board[tile_x][y]) break;
        word += new_board[tile_x][y]->letter;
    }

    return word;
}


}

