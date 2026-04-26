#include "AturDaduCommand.hpp"
#include "../Exceptions/DiceExceptions.hpp"
#include "../Dice/Dice.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Player/Player.hpp"
#include <iostream>

AturDaduCommand::AturDaduCommand(GameMaster& gm, Player* player, Dice& d, int v1, int v2) 
    : gameMaster(gm), currentPlayer(player), dice(d), targetVal1(v1), targetVal2(v2) {}

void AturDaduCommand::execute(GameMaster& gm) {
    (void)gm;

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

    // 1. Validasi rentang nilai dadu
    if (targetVal1 < 1 || targetVal1 > 6 || targetVal2 < 1 || targetVal2 > 6) {
        throw InvalidDiceValueException(targetVal1, targetVal2);
    }

    std::cout << currentPlayer->getUsername() << " mengatur dadu secara manual..." << std::endl;

    // 2. Atur dadu secara manual
    dice.setManual(targetVal1, targetVal2);

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

        dice.resetConsecutiveDoubles();
        gameMaster.sendPlayerToJail(currentPlayer);
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
        gameMaster.setExtraTurn(false);
    }

    GamePhase currentPhase = gameMaster.getState().getPhase();
    if (currentPhase != GamePhase::AWAITING_BUY &&
        currentPhase != GamePhase::AUCTION      &&
        currentPhase != GamePhase::BANKRUPTCY   &&
        currentPhase != GamePhase::AWAITING_FESTIVAL &&
        currentPhase != GamePhase::AWAITING_TAX &&
        currentPhase != GamePhase::AWAITING_RENT &&
        currentPhase != GamePhase::AWAITING_PBM &&
        currentPhase != GamePhase::SHOW_CARD &&
        currentPhase != GamePhase::AWAITING_JAIL &&
        currentPhase != GamePhase::GAME_OVER) {
        gameMaster.getState().setPhase(GamePhase::PLAYER_TURN);
    }
}
