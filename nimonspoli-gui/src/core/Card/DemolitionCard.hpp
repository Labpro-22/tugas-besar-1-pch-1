#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class DemolitionCard : public SkillCard
{
public:
    DemolitionCard();
    ~DemolitionCard();
    void execute(Player &p, GameState &gs) override;
};

#endif