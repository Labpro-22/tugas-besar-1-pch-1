#include "ShieldCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

ShieldCard::ShieldCard() : SkillCard("Kebal tagihan atau sanksi selama 1 turn", "ShieldCard")
{
}

ShieldCard::ShieldCard(const string &type, const string &description, bool used, int duration) : SkillCard(type, description, used), duration(duration)
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

string ShieldCard::successMessage() const
{
    return "ShieldCard diaktifkan. Anda kebal terhadap tagihan sewa, pajak, atau sanksi merugikan selama giliran ini.";
}