#include "AuctionManager.hpp"
#include <iostream>

AuctionManager::AuctionManager()
    : auctionedProperty(nullptr), highestBidder(nullptr), currentBid(0),
      currentBidderIndex(0), consecutivePasses(0), isAuctionOngoing(false) {}

void AuctionManager::setupAuction(Property *prop, Player *initiator,
                                   const std::vector<Player *> &players)
{
    if (prop == nullptr) return;

    auctionedProperty = prop;
    isAuctionOngoing  = true;
    highestBidder     = nullptr;
    currentBid        = 0;
    consecutivePasses = 0;
    activeParticipants.clear();

    int initiatorIdx = -1;
    for (Player *p : players)
    {
        if (p->getStatus() != PlayerStatus::BANKRUPT)
        {
            activeParticipants.push_back(p);
            if (p == initiator)
                initiatorIdx = (int)activeParticipants.size() - 1;
        }
    }

    // Giliran pertama = pemain SETELAH initiator
    if (!activeParticipants.empty())
        currentBidderIndex = (initiatorIdx + 1) % (int)activeParticipants.size();
}

Player *AuctionManager::getCurrentBidder() const
{
    if (activeParticipants.empty()) return nullptr;
    return activeParticipants[currentBidderIndex];
}

bool AuctionManager::placeBid(int amount)
{
    if (!isAuctionOngoing || activeParticipants.empty()) return false;

    Player *bidder = activeParticipants[currentBidderIndex];

    if (amount > currentBid && bidder->getBalance() >= amount)
    {
        currentBid        = amount;
        highestBidder     = bidder;
        consecutivePasses = 0;
        currentBidderIndex =
            (currentBidderIndex + 1) % (int)activeParticipants.size();
        return true;
    }
    return false;
}

void AuctionManager::passBid()
{
    if (!isAuctionOngoing) return;
    consecutivePasses++;
    currentBidderIndex =
        (currentBidderIndex + 1) % (int)activeParticipants.size();
}

bool AuctionManager::isAuctionOver() const
{
    if ((int)activeParticipants.size() <= 1) return true;

    // Ada penawar → selesai jika (N-1) PASS beruntun
    if (highestBidder != nullptr &&
        consecutivePasses >= (int)activeParticipants.size() - 1)
        return true;

    // Belum ada penawar → batal jika semua N pemain PASS
    if (highestBidder == nullptr &&
        consecutivePasses >= (int)activeParticipants.size())
        return true;

    return false;
}

void AuctionManager::closeAuction(Bank &centralBank)
{
    if (!isAuctionOngoing) return;

    if (highestBidder != nullptr && auctionedProperty != nullptr)
    {
        *highestBidder -= currentBid;
        auctionedProperty->setOwner(highestBidder->getUsername());
        auctionedProperty->setStatus(PropertyStatus::OWNED);
        highestBidder->addProperty(auctionedProperty);
    }
    // Jika tidak ada pemenang → properti tetap di Bank (tidak ada aksi)

    isAuctionOngoing  = false;
    auctionedProperty = nullptr;
    highestBidder     = nullptr;
}

// ── Getter ────────────────────────────────────────────────────────────────────

bool AuctionManager::isOngoing() const
{
    return isAuctionOngoing;
}

int AuctionManager::getCurrentBid() const
{
    return currentBid;
}

Player *AuctionManager::getHighestBidder() const
{
    return highestBidder;
}

Property *AuctionManager::getAuctionedProperty() const
{
    return auctionedProperty;
}