#include "TeleportCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"

TeleportCard::TeleportCard() : SkillCard("Pindah ke petak manapun", "TeleportCard"), targetPosition(-1)
{
}

TeleportCard::TeleportCard(const string &type, const string &description, bool used) : SkillCard(type, description, used), targetPosition(-999)
{
}

TeleportCard::~TeleportCard()
{
}

void TeleportCard::setTargetPosition(int pos, GameState &gs)
{
    int boardSize = gs.getBoard()->getSize();
    if (pos >= 0 && pos < boardSize)
        targetPosition = pos;
}

int TeleportCard::getTargetPosition() const { return targetPosition; }

void TeleportCard::execute(Player &p, GameState &gs)
{
    if (targetPosition == -1)
        throw runtime_error("TeleportCard gagal digunakan: target petak belum dipilih.");

    GameMaster *gm = gs.getGameMaster();

    if (!gm)
        throw runtime_error("TeleportCard gagal digunakan: GameMaster tidak tersedia.");

    gm->teleportPlayer(&p, targetPosition, true);

    targetPosition = -1;
    markUsed();
}

string TeleportCard::successMessage() const
{
    return "TeleportCard digunakan. Pilih petak tujuan pada papan untuk memindahkan bidak.";
}