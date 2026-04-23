#include "AuctionManager.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Bank/Bank.hpp"
#include <iostream>

AuctionManager::AuctionManager()
    : auctionedProperty(nullptr), highestBidder(nullptr),
      currentBid(0), isAuctionOngoing(false) {}

void AuctionManager::setupAuction(Property *prop, const std::vector<Player *> &players)
{
    if (prop == nullptr)
        return;

    auctionedProperty = prop;
    isAuctionOngoing = true;
    highestBidder = nullptr;

    // Berdasarkan spesifikasi, lelang dimulai dari harga minimal tertentu
    // Misal 10% dari harga dasar tanah (getLandPrice())
    currentBid = prop->getPurchasePrice() * 0.1;

    // Filter hanya pemain yang tidak bangkrut untuk ikut lelang
    activeParticipants.clear();
    for (Player *p : players)
    {
        if (p->getStatus() != PlayerStatus::BANKRUPT)
        {
            activeParticipants.push_back(p);
        }
    }
}

bool AuctionManager::placeBid(Player *bidder, int amount)
{
    if (!isAuctionOngoing || bidder == nullptr)
        return false;

    // Validasi: Bid harus lebih tinggi dari bid saat ini dan pemain harus punya uang cukup
    if (amount > currentBid && bidder->getBalance() >= amount)
    {
        currentBid = amount;
        highestBidder = bidder;
        return true;
    }

    return false;
}

void AuctionManager::closeAuction(Bank &centralBank)
{
    if (!isAuctionOngoing)
        return;

    if (highestBidder != nullptr && auctionedProperty != nullptr)
    {
        // Pemain membayar ke bank melalui koordinasi AuctionManager
        highestBidder -= currentBid;

        // Atur kepemilikan properti ke pemenang lelang
        auctionedProperty->setOwner(highestBidder->getUsername());

        // Tambahkan properti ke daftar aset pemain (menggunakan method di Player)
        highestBidder->addProperty(auctionedProperty);

        std::cout << "Lelang berakhir! " << highestBidder->getUsername()
                  << " memenangkan " << auctionedProperty->getName()
                  << " seharga " << currentBid << std::endl;
    }
    else
    {
        std::cout << "Lelang berakhir tanpa pemenang." << std::endl;
    }

    // Reset state lelang
    isAuctionOngoing = false;
    auctionedProperty = nullptr;
    highestBidder = nullptr;
}

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