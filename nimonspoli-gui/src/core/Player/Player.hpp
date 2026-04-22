#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include "../Card/Card.hpp"
#include "../Property/Property.hpp"

// Forward declaration untuk Tile — hindari include Board.hpp di sini
// (Board.hpp include GameState.hpp yang include CardDeck.hpp dst → rantai panjang)
class Tile;

class Player {
private: 
    std::string id;
    int money;
    Tile* currPetak;
    std::vector<Property*> listProperty;
    std::vector<Card*> listCard;
    std::string status;         // "ACTIVE" | "JAILED" | "BANKRUPT"

public: 
    Player(std::string id, int money, Tile* currPetak,
           std::vector<Property*> listProperty,
           std::vector<Card*> listCard,
           std::string status) 
        : id(id), money(money), currPetak(currPetak),
          listProperty(listProperty), listCard(listCard), status(status) {}

    // ── Identitas ────────────────────────────────
    std::string getID() const { return id; }
    std::string getId() const { return id; }   // alias — konsisten dengan kode lama

    // ── Uang ─────────────────────────────────────
    int getMoney() const { return money; }

    Player& operator=(int amount) { money = amount;  return *this; }
    Player& operator+(int amount) { money += amount; return *this; }
    Player& operator-(int amount) { money -= amount; return *this; }

    int getWealth() const;   // uang + harga beli semua properti + bangunan

    // ── Posisi ────────────────────────────────────
    // Diperlukan oleh GameMaster::movePlayer()
    Tile* getCurrPetak() const;
    void  setCurrPetak(Tile* tile);
    void  move(int steps);   // akan diimplementasi setelah Board selesai

    // ── Properti ──────────────────────────────────
    void      addProperty(Property* p) { listProperty.push_back(p); }
    Property* getPropertyAt(int pos) const { return listProperty[pos]; }
    int       getPropertyNum() const;
    void      showProperty() const;

    // ── Kartu ─────────────────────────────────────
    void  addCard(Card* newCard);
    Card* getCardAt(int pos) const { return listCard[pos]; }
    Card* removeCardAt(int pos);
    int   getCardNum() const;   // dipakai handleWinner() di GameMaster

    // ── Status ────────────────────────────────────
    void        setStatus(std::string newStatus);
    std::string getStatus() const { return status; }
};

#endif