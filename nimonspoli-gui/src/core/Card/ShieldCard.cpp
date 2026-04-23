#include "ShieldCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

ShieldCard::ShieldCard() : SkillCard("Kebal tagihan atau sanksi selama 1 turn", "ShieldCard")
{
}

ShieldCard::~ShieldCard()
{
}

int ShieldCard::getDuration() const
{
    return duration;
}

void ShieldCard::decreaseDuration()
{
    duration--;
}

void ShieldCard::execute(Player &p, GameState &gs)
{
    p.activateShield();
}