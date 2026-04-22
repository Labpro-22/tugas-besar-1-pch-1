#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class DemolitionCard : public SkillCard
{
public:
    DemolitionCard();
    DemolitionCard(const string &type, const string &description, bool used);
    ~DemolitionCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif