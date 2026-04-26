#include "LassoCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"

#include <stdexcept>

LassoCard::LassoCard() : SkillCard("Tarik lawan ke petakmu", "LassoCard")
{
}

LassoCard::LassoCard(const string &type, const string &description, bool used)
    : SkillCard(type, description, used), targetPlayerUsername("")
{
}

LassoCard::~LassoCard()
{
}

void LassoCard::setTargetPlayerUsername(const string &target)
{
    this->targetPlayerUsername = target;
}

void LassoCard::execute(Player &p, GameState &gs)
{
    Board *board = gs.getBoard();
    if (!board)
        throw runtime_error("LassoCard gagal digunakan: Board tidak tersedia.");

    int currPos = p.getPosition();
    int boardSize = board->getSize();

    Player *target = nullptr;

    vector<Player *> players = gs.getActivePlayers();

    if (!targetPlayerUsername.empty())
    {
        for (Player *other : players)
        {
            if (!other)
                continue;

            if (other == &p)
                continue;
            if (other->isInJail())
                break;

            if (other->getUsername() == targetPlayerUsername)
            {
                if (other->getStatus() == PlayerStatus::JAILED)
                    throw runtime_error("LassoCard gagal digunakan: pemain target sedang dipenjara.");

                int steps = (other->getPosition() - currPos + boardSize) % boardSize;

                if (steps > 0)
                    target = other;

                break;
            }
        }
    }
    else
    {
        int minSteps = boardSize + 1;

        for (Player *other : players)
        {
            if (!other || other == &p)
                continue;

            int steps = (other->getPosition() - currPos + boardSize) % boardSize;

            if (steps > 0 && steps < minSteps)
            {
                minSteps = steps;
                target = other;
            }
        }
    }

    if (!target)
        throw runtime_error("LassoCard gagal digunakan: tidak ada pemain target yang valid.");

    GameMaster *gm = gs.getGameMaster();
    if (!gm)
        throw runtime_error("LassoCard gagal digunakan: GameMaster tidak tersedia.");

    gm->teleportPlayer(target, currPos, false);

    targetPlayerUsername = "";
    markUsed();
}

string LassoCard::successMessage() const
{
    return "LassoCard digunakan. Pilih pemain lawan yang ingin ditarik ke petak Anda.";
}