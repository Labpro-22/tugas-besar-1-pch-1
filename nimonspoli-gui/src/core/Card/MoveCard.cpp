#include "MoveCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"

MoveCard::MoveCard() : MoveCard(rand() % 6 + 1) {}

MoveCard::MoveCard(int steps) : SkillCard("Maju " + to_string(steps) + " Petak", "MoveCard"), steps(steps) {}

MoveCard::MoveCard(const string &type, const string &description, bool used, int steps) : SkillCard(type, description, used), steps(steps) {}

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

    if (!gm)
        throw runtime_error("GameMaster tidak tersedia.");

    Board *board = gs.getBoard();

    if (!board)
        throw runtime_error("Board tidak tersedia.");

    int boardSize = board->getSize();

    if (boardSize <= 0)
        throw runtime_error("Ukuran board tidak valid.");

    int curPos = p.getPosition();
    int targetPos = (curPos + steps) % boardSize;

    gm->teleportPlayer(&p, targetPos, true);

    markUsed();
}

string MoveCard::successMessage() const
{
    return "MoveCard digunakan. Bidak Anda bergerak maju " +
           to_string(getSteps()) +
           " petak.";
}