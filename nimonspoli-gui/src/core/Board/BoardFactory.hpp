#pragma once
#include "../utils/ConfigLoader.hpp"
#include "../Card/CardDeck.hpp"
#include "../Card/ChanceCard.hpp"
#include "../Card/GeneralFundCard.hpp"
#include "Board.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
using namespace std;

class BoardFactory
{
public:
    BoardFactory();
    static Board *createBoard(const vector<PropertyData> &propertyDataList,
                              const vector<ActionData> &actionDataList,
                              SpecialConfig &config,
                              CardDeck<Card> *chanceCard,
                              CardDeck<Card> *genFundCard,
                              const std::vector<std::unique_ptr<Property>> &properties);
};