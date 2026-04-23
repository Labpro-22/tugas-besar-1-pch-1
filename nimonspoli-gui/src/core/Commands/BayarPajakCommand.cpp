#include "BayarPajakCommand.hpp"
#include "../Player/Player.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../utils/ConfigLoader.hpp"
#include <iostream>

// ─────────────────────────────────────────────
//  Konstruktor
// ─────────────────────────────────────────────

BayarPajakCommand::BayarPajakCommand(Player* p, TaxConfig taxConfig,
                                     std::string taxType, int option)
    : currentPlayer(p), taxConfig(taxConfig), taxType(taxType), option(option) {}



// ─────────────────────────────────────────────
//  execute()
// ─────────────────────────────────────────────

void BayarPajakCommand::execute(GameMaster& gm) {
    if (!currentPlayer) {
        std::cerr << "[BayarPajakCommand] Error: player null." << std::endl;
        return;
    }

    GameState& gs = gm.getState();

    // Nilai pajak dari TaxConfig (dibaca ConfigLoader dari tax.txt)
    int taxAmount = 0;

    // ── Hitung nominal pajak ──────────────────────────────────────────────────
    if (taxType == "PPH") {
        if (option == 1) {
            taxAmount = static_cast<int>(taxConfig.pphFlat);
            std::cout << "[DEBUG] PPH flat: M" << taxAmount << std::endl;
        } else {
            // Opsi 2: persentase dari total kekayaan
            // Kekayaan HARUS dihitung SETELAH user memilih opsi (sesuai spesifikasi)
            int wealth = currentPlayer->getWealth();
            taxAmount  = static_cast<int>(wealth * (taxConfig.pphPercentage / 100.0));
            std::cout << "[DEBUG] PPH " << taxConfig.pphPercentage << "% dari kekayaan M"
                      << wealth << " = M" << taxAmount << std::endl;
        }
    } else if (taxType == "PBM") {
        taxAmount = static_cast<int>(taxConfig.pbmFlat);
        std::cout << "[DEBUG] PBM flat: M" << taxAmount << std::endl;
    } else {
        std::cerr << "[BayarPajakCommand] taxType tidak dikenal: " << taxType << std::endl;
        return;
    }

    // ── Proses pembayaran ─────────────────────────────────────────────────────
    if (currentPlayer->canAfford(taxAmount)) {
        *currentPlayer -= taxAmount;
        std::cout << "[DEBUG] " << currentPlayer->getUsername()
                  << " membayar pajak M" << taxAmount
                  << ". Saldo: M" << currentPlayer->getBalance() << std::endl;
    } else {
        // Tidak mampu bayar → serahkan ke alur kebangkrutan di GameMaster
        std::cout << "[DEBUG] " << currentPlayer->getUsername()
                  << " tidak mampu bayar pajak M" << taxAmount
                  << ". Saldo: M" << currentPlayer->getBalance()
                  << ". Proses kebangkrutan dimulai." << std::endl;

        gs.setPhase(GamePhase::BANKRUPTCY);
        // GameMaster akan mendeteksi fase BANKRUPTCY dan menjalankan alur likuidasi
        
    }
}