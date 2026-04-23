#ifndef AUCTIONMANAGER_HPP
#define AUCTIONMANAGER_HPP

#include <vector>
#include <string>
#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"
#include "../Property/Property.hpp"

class AuctionManager {
private:
    Property* auctionedProperty;
    Player* highestBidder;
    int currentBid;
    std::vector<Player*> activeParticipants; // Hanya yang tidak bangkrut
    int currentBidderIndex;                  // Melacak giliran siapa
    int consecutivePasses;                   // Menghitung jumlah PASS berturut-turut
    bool isAuctionOngoing;

public:
    AuctionManager();

    // Diperbarui: Tambahkan 'initiator' agar lelang dimulai dari pemain SETELAHNYA
    void setupAuction(Property* prop, Player* initiator, const std::vector<Player*>& players);

    // Diperbarui: Hapus parameter Player*, Manager tahu giliran siapa dari currentBidderIndex
    bool placeBid(int amount);
    void passBid();

    void closeAuction(Bank& centralBank);

    bool isOngoing() const;
    bool isAuctionOver() const;             // Penentu apakah lelang sudah selesai
    
    int getCurrentBid() const;
    Player* getHighestBidder() const;
    Property* getAuctionedProperty() const;
    Player* getCurrentBidder() const;       // Mendapatkan penawar yang sedang aktif
};

#endif