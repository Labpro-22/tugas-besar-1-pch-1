#ifndef GAME_RESULT_HPP
#define GAME_RESULT_HPP

#pragma once
#include <string>
#include "../../../lib/raylib/include/raylib.h"

struct PlayerResult {
    std::string username;
    int         money;
    int         propertyCount;
    int         cardCount;
    bool        bankrupt;
    int         rank;
    bool        isWinner;
    Color       color;
};

enum class WinScenario {
    MAX_TURN,
    BANKRUPTCY
};

#endif