#ifndef TELEPORT_CARD_HPP
#define TELEPORT_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class TeleportCard : public SkillCard
{
public:
    TeleportCard();
    TeleportCard(const string &type, const string &description, bool used);
    ~TeleportCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif