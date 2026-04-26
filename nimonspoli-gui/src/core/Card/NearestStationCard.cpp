#include "NearestStationCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"

NearestStationCard::NearestStationCard() : ChanceCard("Pergi ke stasiun terdekat.")
{
}

NearestStationCard::NearestStationCard(const string &type, const string &description) : ChanceCard(description)
{
}

NearestStationCard::~NearestStationCard()
{
}

void NearestStationCard::execute(Player &p, GameState &gs)
{
    Board *board = gs.getBoard();
    int boardSize = board->getSize();
    int currPos = p.getPosition();

    int nearestIdx = -1;
    int minSteps = boardSize + 1;

    for (int i = 0; i < boardSize; ++i)
    {
        Tile *t = board->getTile(i);
        if (t && t->getTileType() == "RAILROAD")
        {
            int steps = (i - currPos + boardSize) % boardSize;
            if (steps == 0)
                steps = boardSize;

            if (steps < minSteps)
            {
                minSteps = steps;
                nearestIdx = i;
            }
        }
    }

    if (nearestIdx >= 0)
    {
        GameMaster *gm = gs.getGameMaster();
        if (gm)
            gm->movePlayer(&p, minSteps);
    }
}