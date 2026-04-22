#include "Bank.hpp"
//#include "Player.hpp" // Membutuhkan definisi lengkap dari kelas Player untuk interaksi uang
#include <iostream>

// Inisialisasi stok bangunan (secara default di Monopoli biasanya 32 rumah dan 12 hotel)
Bank::Bank() : housesCount(32), hotelsCount(12) {}

Bank::Bank(int initialHouses, int initialHotels) : housesCount(initialHouses), hotelsCount(initialHotels) {}

void Bank::payPlayer(Player* p, int amount) {
    if (p != nullptr && amount > 0) {
        // Berdasarkan diagram kelas Player, terdapat operator+(int) yang me-return Player&
        // Operasi ini menambahkan uang ke dalam state pemain.
        *p + amount; 
    }
}

void Bank::receiveFromPlayer(Player* p, int amount) {
    if (p != nullptr && amount > 0) {
        // Berdasarkan diagram kelas Player, terdapat operator-(int) yang me-return Player&
        // Operasi ini mengurangi uang dari state pemain.
        *p - amount; 
    }
}

void Bank::confesticatePlayerAssets(Player* p) {
    if (p != nullptr) {
        // 1. Bank menyita sisa uang tunai yang dimiliki pemain (jika ada)
        int remainingMoney = p->getMoney();
        if (remainingMoney > 0) {
            receiveFromPlayer(p, remainingMoney);
        }
        
        // 2. Mengubah status pemain menjadi BANKRUPT
        p->setStatus("BANKRUPT");

        /* * TUGAS LANJUTAN UNTUK M2:
         * Di bagian ini, Bank harus menangani pelelangan aset dan penghancuran bangunan.
         * perlu mengiterasi seluruh properti yang dimiliki pemain (p->getPropertyAt()), lalu:
         * - Menjual seluruh bangunan (menambahkan housesCount/hotelsCount kembali ke Bank).
         * - Memanggil AuctionManager untuk melelang lahan ke pemain lain yang masih aktif.
         * - Menghapus kepemilikan (clearOwner) dari properti tersebut.
         */
    }
}

int Bank::getHousesCount() const {
    return housesCount;
}

void Bank::setHousesCount(int count) {
    housesCount = count;
}

int Bank::getHotelsCount() const {
    return hotelsCount;
}

void Bank::setHotelsCount(int count) {
    hotelsCount = count;
}