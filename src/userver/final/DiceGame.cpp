#include "DiceGame.hpp"

Combinations DiceGame::CalculateSequences(std::vector<int> &dices) {
    Combinations c;
    std::map<int,int> freq;

    for (int d : dices) {
        freq[d]++;
    }

    // Ones-Sixes
    for (int i = 1; i <= 6; i++) {
        int sum = i * freq[i];
        switch (i) {
            case 1: c.ones = sum; break;
            case 2: c.twos = sum; break;
            case 3: c.threes = sum; break;
            case 4: c.fours = sum; break;
            case 5: c.fives = sum; break;
            case 6: c.sixes = sum; break;
        }
    }

    // Chance 
    c.chance = std::accumulate(dices.begin(), dices.end(), 0);

    // Группы
    std::vector<int> counts;
    for (auto& [val, cnt] : freq) counts.push_back(cnt);
    std::sort(counts.rbegin(), counts.rend());

    // One pair
    for (auto& [val, cnt] : freq) {
        if (cnt >= 2) c.one_pair = std::max(c.one_pair, val * 2);
    }

    // Two pairs
    {
        std::vector<int> pairs;
        for (auto& [val, cnt] : freq) {
            if (cnt >= 2) pairs.push_back(val);
        }
        if (pairs.size() >= 2) {
            std::sort(pairs.rbegin(), pairs.rend());
            c.two_pairs = pairs[0] * 2 + pairs[1] * 2;
        }
    }

    // Three of a kind
    for (auto& [val, cnt] : freq) {
        if (cnt >= 3) c.three_of_kind = std::max(c.three_of_kind, val * 3);
    }

    // Four of a kind
    for (auto& [val, cnt] : freq) {
        if (cnt >= 4) c.four_of_kind = std::max(c.four_of_kind, val * 4);
    }

    // Full House
    bool has3 = false, has2 = false;
    int val3 = 0, val2 = 0;
    for (auto& [val, cnt] : freq) {
        if (cnt >= 3) { has3 = true; val3 = val; }
        else if (cnt >= 2) { has2 = true; val2 = val; }
    }
    if (has3 && has2) {
        c.full_house = val3 * 3 + val2 * 2;
    }

    // Straights
    {
        std::vector<int> sorted = dices;
        std::sort(sorted.begin(), sorted.end());
        sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
        if (sorted == std::vector<int>{1,2,3,4} or sorted == std::vector<int>{2,3,4,5} or sorted == std::vector<int>{3,4,5,6}) c.small_straight = 20;
        if (sorted == std::vector<int>{1,2,3,4,5} or sorted == std::vector<int>{2,3,4,5,6}) c.large_straight = 30;
    }

    // Yahtzee (5 одинаковых)
    for (auto& [val, cnt] : freq) {
        if (cnt == 5) c.five_of_kind = 50;
    }

    return c;
}

std::vector<int> DiceGame::RollDices() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(1, 6);
    
    std::vector<int> dices(5);
    for (int i = 0; i < 5; i++) {
        dices[i] = dist(gen);
    }
    
    return dices;
}