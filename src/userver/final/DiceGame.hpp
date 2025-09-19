#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <random>

struct Combinations {
    int ones = 0, twos = 0, threes = 0, fours = 0, fives = 0, sixes = 0;
    int one_pair = 0;
    int two_pairs = 0;
    int three_of_kind = 0;
    int four_of_kind = 0;
    int full_house = 0;
    int small_straight = 0;
    int large_straight = 0;
    int five_of_kind = 0;
    int chance = 0;
};

class DiceGame {

public:
    std::vector<int> RollDices();
    Combinations CalculateSequences(std::vector<int> dices);
    
};