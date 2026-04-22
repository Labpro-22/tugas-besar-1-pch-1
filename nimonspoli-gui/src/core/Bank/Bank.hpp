#ifndef BANK_HPP
#define BANK_HPP

// Forward declaration untuk mencegah circular dependency dengan header Player, nanti dihapus lagi
#include "../Player/Player.hpp"

class Bank {
private:
    int housesCount;
    int hotelsCount;

public:
    // Konstruktor
    Bank();
    Bank(int initialHouses, int initialHotels);

    // Memberikan uang kepada player (misal saat gadai properti atau lewat petak GO)
    void payPlayer(Player* p, int amount);

    // Menerima uang dari player 
    void receiveFromPlayer(Player* p, int amount);

    // Menyita aset dan melelang properti pemain yang bangkrut
    // Penamaan method dipertahankan sesuai typo pada diagram kelas (confesticate)
    void confesticatePlayerAssets(Player* p); 

    // Getter dan Setter untuk kemudahan manajemen stok bangunan di kemudian hari
    int getHousesCount() const;
    void setHousesCount(int count);
    
    int getHotelsCount() const;
    void setHotelsCount(int count);
};

#endif