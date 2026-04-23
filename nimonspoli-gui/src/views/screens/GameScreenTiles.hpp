#pragma once
#include "GameScreen.hpp"

// Definisi TILE_DEFS — diakses semua GameScreen*.cpp
inline const TileDef TILE_DEFS[40] = {
    // BOTTOM ROW right→left
    {"GO",  "BOTTOM", true},   // 0
    {"GRT", "BOTTOM", false},  // 1
    {"DNU", "BOTTOM", false},  // 2
    {"TSK", "BOTTOM", false},  // 3
    {"PPH", "BOTTOM", false},  // 4
    {"GBR", "BOTTOM", false},  // 5
    {"BGR", "BOTTOM", false},  // 6
    {"FES", "BOTTOM", false},  // 7
    {"DPK", "BOTTOM", false},  // 8
    {"BKS", "BOTTOM", false},  // 9
    {"PEN", "BOTTOM", true},   // 10

    // LEFT COLUMN bottom→top
    {"MGL", "LEFT", false},    // 11
    {"PLN", "LEFT", false},    // 12
    {"SOL", "LEFT", false},    // 13
    {"YOG", "LEFT", false},    // 14
    {"STB", "LEFT", false},    // 15
    {"MAL", "LEFT", false},    // 16
    {"SMG", "LEFT", false},    // 17
    {"DNU", "LEFT", false},    // 18
    {"SBY", "LEFT", false},    // 19

    // TOP ROW left→right
    {"BBP", "TOP", true},      // 20
    {"MKS", "TOP", false},     // 21
    {"KSP", "TOP", false},     // 22
    {"BLP", "TOP", false},     // 23
    {"MND", "TOP", false},     // 24
    {"TUG", "TOP", false},     // 25
    {"PLB", "TOP", false},     // 26
    {"PKB", "TOP", false},     // 27
    {"PAM", "TOP", false},     // 28
    {"MED", "TOP", false},     // 29
    {"PPJ", "TOP", true},      // 30

    // RIGHT COLUMN top→bottom
    {"BDG", "RIGHT", false},   // 31
    {"DEN", "RIGHT", false},   // 32
    {"FES", "RIGHT", false},   // 33
    {"MTR", "RIGHT", false},   // 34
    {"GUB", "RIGHT", false},   // 35
    {"KSP", "RIGHT", false},   // 36
    {"JKT", "RIGHT", false},   // 37
    {"PBM", "RIGHT", false},   // 38
    {"IKN", "RIGHT", false},   // 39
};