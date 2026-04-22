#include "LelangCommand.hpp"
#include "../AuctionManager/AuctionManager.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Property/Property.hpp"
#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"
#include <iostream>

LelangCommand::LelangCommand(AuctionManager& am, GameMaster& gm, Bank& b, Property* prop)
    : auctionManager(am), gameMaster(gm), bank(b), targetProperty(prop) {}

void LelangCommand::execute() {
    if (targetProperty == nullptr) return;

    std::cout << "\n=== MEMULAI LELANG: " << targetProperty->getName() << " ===" << std::endl;
    
    // 1. Inisialisasi sesi lelang di AuctionManager
    // Mengambil daftar semua pemain dari GameMaster
    std::vector<Player*> allPlayers = gameMaster.getAllPlayers();
    auctionManager.setupAuction(targetProperty, allPlayers);

    bool auctionActive = true;
    while (auctionActive && auctionManager.isOngoing()) {
        std::cout << "\nHarga saat ini: " << auctionManager.getCurrentBid() << std::endl;
        if (auctionManager.getHighestBidder() != nullptr) {
            std::cout << "Penawar tertinggi: " << auctionManager.getHighestBidder()->getName() << std::endl;
        }

        bool anyBidInThisRound = false;

        // 2. Iterasi penawaran untuk setiap pemain yang masih aktif (tidak bangkrut)
        for (Player* p : allPlayers) {
            if (p->getStatus() == "BANKRUPT") continue;

            std::cout << "\nGiliran " << p->getName() << " (Uang: " << p->getMoney() << ")" << std::endl;
            std::cout << "Masukkan tawaran (0 untuk pass): ";
            
            int bidInput;
            if (!(std::cin >> bidInput)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                bidInput = 0;
            }

            if (bidInput > 0) {
                if (auctionManager.placeBid(p, bidInput)) {
                    std::cout << "Tawaran diterima!" << std::endl;
                    anyBidInThisRound = true;
                } else {
                    std::cout << "Tawaran tidak valid (harus lebih tinggi dan uang cukup)." << std::endl;
                }
            }
        }

        // Jika dalam satu putaran penuh tidak ada yang menaikkan bid, lelang selesai
        if (!anyBidInThisRound) {
            auctionActive = false;
        }
    }

    // 3. Finalisasi lelang: Transfer kepemilikan dan uang ke Bank
    auctionManager.closeAuction(bank);
    std::cout << "=== LELANG SELESAI ===\n" << std::endl;
}