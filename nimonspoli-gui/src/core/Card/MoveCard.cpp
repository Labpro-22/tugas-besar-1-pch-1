#include "MoveCard.hpp"

MoveCard::MoveCard()
{
}

MoveCard::MoveCard(const string &type, const string &description, bool used, int steps) : SkillCard(type, description, used), steps(steps)
{
}

MoveCard::~MoveCard()
{
}

void MoveCard::execute(Player &p, GameMaster &g) {}