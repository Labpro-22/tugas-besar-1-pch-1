#include "LassoCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"

LassoCard::LassoCard() : SkillCard("Tarik lawan ke petakmu", "LassoCard")
{
}

LassoCard::~LassoCard()
{
}

void LassoCard::execute(Player &p, GameState &gs)
{
    int currPos = p.getPosition();
    int boardSize = gs.getBoard()->getSize();
    vector<Player *> players = gs.getActivePlayers();

    Player *target = nullptr;
    int minSteps = boardSize + 1;

    for (Player *other : players)
    {
        if (other == &p)
            continue;
        int steps = (other->getPosition() - currPos + boardSize) % boardSize;
        if (steps > 0 && steps < minSteps)
        {
            minSteps = steps;
            target = other;
        }
    }

    if (target)
    {
        GameMaster *gm = gs.getGameMaster();
        if (gm)
            gm->teleportPlayer(target, currPos);
    }
}