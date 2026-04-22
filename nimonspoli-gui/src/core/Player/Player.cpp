#include "Player.hpp"

int Player::getWealth() const 
{
    int totalWealth = money;
    for (Property* prop : listProperty) {
        // Kekayaan = uang tunai + harga BELI semua properti + harga beli bangunan
        // (bukan mortgageValue — sesuai spesifikasi PPH)
        totalWealth += static_cast<int>(prop->getPurchasePrice());
    }
    return totalWealth;
}

void Player::move(int steps)
{
    // Implementasi menunggu Board selesai.
    // Yang perlu dilakukan:
    //   1. Hitung indeks baru = (indeksSekarang + steps) % board.getSize()
    //   2. Set currPetak = board.getTile(indeksBaru)
    // Saat ini dikosongkan — akan diisi setelah Board tersedia.
    (void)steps;
}

// Getter posisi saat ini sebagai indeks integer
// Dipanggil oleh GameMaster::movePlayer untuk tahu posisi pemain
Tile* Player::getCurrPetak() const
{
    return currPetak;
}

void Player::setCurrPetak(Tile* tile)
{
    currPetak = tile;
}

int Player::getPropertyNum() const
{
    return static_cast<int>(listProperty.size());
}

void Player::showProperty() const
{
    for (Property* prop : listProperty) {
        // Delegate ke DisplayManager — kosongkan di sini
        // DisplayManager::printPlayerProperties() yang menangani output
        (void)prop;
    }
}

void Player::addCard(Card* newCard)
{
    listCard.push_back(newCard);
}

Card* Player::removeCardAt(int pos)
{
    // Bug fix dari versi sebelumnya:
    // - iterator di-advance sebanyak pos, tapi pos sudah jadi 0 di dalam loop
    // - listCard[pos] selalu index 0 karena pos sudah 0
    if (pos < 0 || pos >= static_cast<int>(listCard.size())) return nullptr;

    Card* ret = listCard[pos];
    listCard.erase(listCard.begin() + pos);
    return ret;
}

void Player::setStatus(std::string newStatus)
{
    status = newStatus;
}

int Player::getCardNum() const
{
    return static_cast<int>(listCard.size());
}