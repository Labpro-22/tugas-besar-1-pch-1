#include "FreeFromJailCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

#include <stdexcept>

FreeFromJailCard::FreeFromJailCard()
    : SkillCard("Keluar dari penjara", "FreeFromJailCard")
{
}

FreeFromJailCard::FreeFromJailCard(const string &type, const string &description, bool used)
    : SkillCard(type, description, used)
{
}

FreeFromJailCard::~FreeFromJailCard()
{
}

void FreeFromJailCard::execute(Player &p, GameState &gs)
{
    if (p.getStatus() != PlayerStatus::JAILED)
    {
        throw runtime_error("FreeFromJailCard hanya bisa digunakan saat pemain berada di penjara.");
    }

    p.releaseFromJail();

    markUsed();
}

string FreeFromJailCard::successMessage() const
{
    return "FreeFromJailCard berhasil digunakan. Anda keluar dari penjara.";
}