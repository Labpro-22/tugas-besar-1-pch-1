#include "MoveCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"

MoveCard::MoveCard() : MoveCard(rand() % 6 + 1) {}

MoveCard::MoveCard(int steps) : SkillCard("Maju " + to_string(steps) + " Petak", "MoveCard"), steps(steps) {}

MoveCard::~MoveCard()
{
}

int MoveCard::getSteps() const
{
    return steps;
}

void MoveCard::execute(Player &p, GameState &gs)
{
    GameMaster *gm = gs.getGameMaster();
    if (gm)
    {
        gm->movePlayer(&p, steps);
    }
}
