#include "AuctionManager.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Bank/Bank.hpp"
#include <iostream>
#include <algorithm>

AuctionManager::AuctionManager()
    : auctionedProperty(nullptr), highestBidder(nullptr),
      currentBid(0), currentParticipantIdx(0),
      consecutivePassCount(0), isAuctionOngoing(false),
      allPassedWithoutBid(false), forcedBidder(nullptr) {}

void AuctionManager::setupAuction(Property* prop, Player* initiator,
                                   const std::vector<Player*>& allPlayers) {
    if (!prop || !initiator) return;

    auctionedProperty    = prop;
    isAuctionOngoing     = true;
    highestBidder        = nullptr;
    currentBid           = 0;
    consecutivePassCount = 0;
    allPassedWithoutBid  = false;
    forcedBidder         = nullptr;

    // Susun urutan peserta mulai dari pemain SETELAH initiator
    activeParticipants.clear();
    auto it = std::find(allPlayers.begin(), allPlayers.end(), initiator);
    int startIdx = 0;
    if (it != allPlayers.end()) {
        startIdx = (int)(it - allPlayers.begin()) + 1;
    }

    int n = (int)allPlayers.size();
    for (int i = 0; i < n; ++i) {
        Player* p = allPlayers[(startIdx + i) % n];
        if (p->getStatus() != PlayerStatus::BANKRUPT)
            activeParticipants.push_back(p);
    }

    currentParticipantIdx = 0;
}

bool AuctionManager::placeBid(Player* bidder, int amount) {
    if (!isAuctionOngoing || !bidder) return false;
    if (bidder != getCurrentParticipant()) return false;

    // Bid harus lebih tinggi dari bid saat ini dan pemain harus mampu
    if (amount <= currentBid || !bidder->canAfford(amount)) return false;

    currentBid           = amount;
    highestBidder        = bidder;
    consecutivePassCount = 0; // reset karena ada BID
    allPassedWithoutBid  = false;
    forcedBidder         = nullptr;

    // Maju ke peserta berikutnya
    currentParticipantIdx = (currentParticipantIdx + 1) % (int)activeParticipants.size();
    return true;
}

void AuctionManager::passBid() {
    if (!isAuctionOngoing) return;

    // Jika sedang dalam mode forced bid, pemain tidak boleh PASS — abaikan
    if (allPassedWithoutBid) return;

    consecutivePassCount++;
    currentParticipantIdx = (currentParticipantIdx + 1) % (int)activeParticipants.size();

    // Jika semua pemain PASS tanpa ada yang BID sama sekali,
    // pemain terakhir yang pass (sekarang currentParticipant) wajib bid
    if (highestBidder == nullptr &&
        consecutivePassCount >= (int)activeParticipants.size()) {
        allPassedWithoutBid = true;
        forcedBidder = getCurrentParticipant();
        std::cout << "[DEBUG] Semua pemain pass tanpa bid. "
                  << (forcedBidder ? forcedBidder->getUsername() : "?")
                  << " wajib melakukan bid." << std::endl;
    }
}

void AuctionManager::closeAuction(Bank& /*centralBank*/) {
    if (!isAuctionOngoing) return;

    if (highestBidder && auctionedProperty) {
        *highestBidder -= currentBid; // potong saldo pemenang
        auctionedProperty->setOwner(highestBidder->getUsername());
        auctionedProperty->setStatus(PropertyStatus::OWNED);
        highestBidder->addProperty(auctionedProperty);

        std::cout << "[DEBUG] Lelang selesai! " << highestBidder->getUsername()
                  << " memenangkan " << auctionedProperty->getName()
                  << " seharga M" << currentBid << std::endl;
    } else {
        std::cout << "[DEBUG] Lelang berakhir tanpa pemenang." << std::endl;
        // Properti tetap milik Bank
    }

    // Reset state
    isAuctionOngoing     = false;
    auctionedProperty    = nullptr;
    highestBidder        = nullptr;
    currentBid           = 0;
    consecutivePassCount = 0;
    allPassedWithoutBid  = false;
    forcedBidder         = nullptr;
    activeParticipants.clear();
}

bool AuctionManager::isAuctionOver() const {
    if (!isAuctionOngoing) return true;
    if (activeParticipants.empty()) return true;

    // Selesai jika (jumlah_peserta - 1) PASS berturut-turut
    return consecutivePassCount >= (int)activeParticipants.size() - 1;
}

bool AuctionManager::isOngoing() const {
    return isAuctionOngoing;
}

int AuctionManager::getCurrentBid() const {
    return currentBid;
}

Player* AuctionManager::getHighestBidder() const {
    return highestBidder;
}

Property* AuctionManager::getAuctionedProperty() const {
    return auctionedProperty;
}

Player* AuctionManager::getCurrentParticipant() const {
    if (activeParticipants.empty()) return nullptr;
    return activeParticipants[currentParticipantIdx];
}

bool AuctionManager::isForcedBid() const {
    return allPassedWithoutBid;
}