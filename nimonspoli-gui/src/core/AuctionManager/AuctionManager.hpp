#ifndef AUCTIONMANAGER_HPP
#define AUCTIONMANAGER_HPP

#include <vector>

#include "../Player/Player.hpp"
#include "../Bank/Bank.hpp"
#include "../Property/Property.hpp"

class AuctionManager {
private:
    Property* auctionedProperty;
    Player* highestBidder;
    int currentBid;
    std::vector<Player*> activeParticipants;
    bool isAuctionOngoing;

public:
    AuctionManager();

    // Memulai sesi lelang baru
    void setupAuction(Property* prop, const std::vector<Player*>& players);

    // Menerima input tawaran dari pemain
    // Mengembalikan true jika tawaran valid dan diterima
    bool placeBid(Player* bidder, int amount);

    // Menyelesaikan lelang, mentransfer kepemilikan, dan mengelola pembayaran ke Bank
    void closeAuction(Bank& centralBank);

    // Status lelang
    bool isOngoing() const;
    int getCurrentBid() const;
    Player* getHighestBidder() const;
    Property* getAuctionedProperty() const;
};

#endif