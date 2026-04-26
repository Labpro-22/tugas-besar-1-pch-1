#ifndef TELEPORT_CARD_HPP
#define TELEPORT_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class TeleportCard : public SkillCard
{
private:
    int targetPosition;

public:
    TeleportCard();
    TeleportCard(const string &type, const string &description, bool used);
    ~TeleportCard();
    void execute(Player &p, GameState &gs) override;
    void setTargetPosition(int pos, GameState &gs);
    int getTargetPosition() const;
    string successMessage() const override;
};

#endif