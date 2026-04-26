#ifndef AUCTIONMANAGER_HPP
#define AUCTIONMANAGER_HPP

#include <vector>
#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"
#include "../Property/Property.hpp"

class AuctionManager {
private:
    Property*            auctionedProperty;
    Player*              highestBidder;
    int                  currentBid;
    std::vector<Player*> activeParticipants; // urutan peserta lelang
    int                  currentParticipantIdx;
    int                  consecutivePassCount; // reset ke 0 setiap ada BID
    bool                 isAuctionOngoing;
    bool                 allPassedWithoutBid;  // semua pass, 1 pemain wajib bid
    Player*              forcedBidder;         // pemain yang wajib bid jika semua pass

public:
    AuctionManager();

    // Memulai sesi lelang baru.
    // initiator = pemain yang memicu lelang; urutan mulai dari pemain SETELAH initiator.
    void setupAuction(Property* prop, Player* initiator,
                      const std::vector<Player*>& allPlayers);

    // Menerima tawaran dari pemain aktif saat ini.
    // Return true jika valid dan diterima.
    bool placeBid(Player* bidder, int amount);

    // Pemain aktif saat ini meng-PASS.
    void passBid();

    // Menyelesaikan lelang: transfer kepemilikan & potong saldo pemenang.
    // Dipanggil dari GUI saat isAuctionOver() == true.
    void closeAuction(Bank& centralBank);

    // Lelang selesai jika (jumlah_peserta - 1) PASS berturut-turut
    bool isAuctionOver() const;

    // Getter untuk GUI
    bool     isOngoing()             const;
    int      getCurrentBid()         const;
    Player*  getHighestBidder()      const;
    Property* getAuctionedProperty() const;
    Player*  getCurrentParticipant() const; // peserta yang sedang giliran
    bool     isForcedBid()           const; // apakah giliran ini wajib BID (tidak boleh PASS)

    };
#endif
