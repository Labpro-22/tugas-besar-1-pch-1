#include "LemparDaduCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Player/Player.hpp"
#include "../Dice/Dice.hpp"
#include <iostream>

LemparDaduCommand::LemparDaduCommand(GameMaster &gm, Player *player, Dice &d)
    : gameMaster(gm), currentPlayer(player), dice(d) {}

void LemparDaduCommand::execute(GameMaster &gm)
{
    (void)gm; // Semua akses GameMaster dilakukan via member gameMaster

    if (currentPlayer == nullptr)
    {
        std::cerr << "Error: Tidak ada pemain yang sedang aktif!" << std::endl;
        return;
    }

    // Pastikan pemain belum melempar dadu di giliran ini
    if (gameMaster.getState().getHasRolled())
    {
        std::cout << "Anda sudah melempar dadu giliran ini." << std::endl;
        return;
    }

    std::cout << currentPlayer->getUsername() << " melempar dadu..." << std::endl;

    dice.rollRandom();

    int val1  = dice.getDaduVal1();
    int val2  = dice.getDaduVal2();
    int total = dice.getTotal();

    std::cout << "Hasil: Dadu 1 = " << val1 << ", Dadu 2 = " << val2
              << " (Total: " << total << ")" << std::endl;

    // Tandai bahwa dadu sudah dilempar giliran ini
    gameMaster.getState().setHasRolled(true);

    // ── Cek 3× double berturut-turut → langsung penjara ──────────────────
    if (dice.getConsecutiveDoubles() >= 3)
    {
        std::cout << "Tiga kali double berturut-turut! "
                  << currentPlayer->getUsername()
                  << " langsung dijebloskan ke Penjara!" << std::endl;

        gameMaster.log(currentPlayer->getUsername(), "JAIL",
                       "3x double berturut-turut → masuk penjara");

        // Reset counter sebelum sendPlayerToJail (yang juga memanggil endCurrentTurn)
        dice.resetConsecutiveDoubles();
        gameMaster.sendPlayerToJail(currentPlayer);

        // sendPlayerToJail sudah memanggil endCurrentTurn() — tidak perlu move pemain
        return;
    }

    // ── Gerakkan pemain ───────────────────────────────────────────────────
    gameMaster.movePlayer(currentPlayer, total);

    // ── Handle double (giliran tambahan) ──────────────────────────────────
    if (dice.isDouble())
    {
        std::cout << "DOUBLE! " << currentPlayer->getUsername()
                  << " mendapat giliran tambahan." << std::endl;
        gameMaster.setExtraTurn(true);

    }
    else
    {
        // Bukan double → reset counter (rollRandom sudah reset, tapi reset eksplisit lebih aman)
        gameMaster.setExtraTurn(false);
    }
    GamePhase currentPhase = gameMaster.getState().getPhase();
    if (currentPhase != GamePhase::AWAITING_BUY &&
        currentPhase != GamePhase::AUCTION      &&
        currentPhase != GamePhase::BANKRUPTCY   &&
        currentPhase != GamePhase::GAME_OVER) {
        gameMaster.getState().setPhase(GamePhase::PLAYER_TURN);
    }

    std::cout << "DEBUG phase setelah execute: " 
          << (int)gameMaster.getState().getPhase() 
          << " hasRolled: " 
          << gameMaster.getState().getHasRolled() 
          << std::endl;
}