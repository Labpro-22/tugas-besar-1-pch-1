#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace std;

class Card;
class SkillCard;
class ShieldCard;
class Property;

enum class PlayerStatus
{
    ACTIVE,
    BANKRUPT,
    JAILED
};

class Player
{
private:
    string username;
    int balance;
    int position; // indeks petak (0-39)
    PlayerStatus status;
    int jailTurns; // sudah berapa giliran di penjara

    vector<Property *> properties;  // daftar properti yang dimiliki
    vector<SkillCard *> skillCards; // kartu kemampuan di tangan (maks 3)

    bool cardUsedThisTurn; // flag pemakaian kartu per giliran
    bool shieldActive;
    string id;

public:
    /**
    + Player(Id: string, Money: int, currPetak: Petak*, listProperty: vector<Property*>, listCard: vector<Card*>, Status: string): Player
    + getID(): string
    + getMoney(): int
    + operator=(amount : int) : Player&
    + operator-(amount : int): Player&
    + operator+(amount : int): Player&
    + getWealth(): int
    + move(steps: int): void
    + getPropertyAt(): Property* // 0 based
    + getPropertyNum(): int
    + showProperty(): void
    + getCardAt(int): Card* // 0 based
    + addCard(Card): void
    + removeCardAt(int): Card* // 0 based
    + setStatus(string): void
    + getStatus(): string
     */
    Player(const std::string &username, int startingBalance);
    ~Player();

    // GET
    string getUsername() const;
    int getBalance() const;
    int getPosition() const;
    PlayerStatus getStatus() const;
    int getJailTurns() const;

    // SET
    void setPosition(int tileIndex);
    void setStatus(PlayerStatus newStatus);
    void setJailTurns(int turns);

    // KEUANGAN
    Player &operator+=(int amount);
    Player &operator-=(int amount);
    bool canAfford(int amount) const;
    bool operator>(const Player &other) const;
    bool operator<(const Player &other) const;

    // Player &operator=(int amount)
    // {
    //     money = amount;
    //     return *this;
    // }
    // Player &operator+(int amount)
    // {
    //     money += amount;
    //     return *this;
    // }
    // Player &operator-(int amount)
    // {
    //     money -= amount;
    //     return *this;
    // }

    // PROPERTI
    void addProperty(Property *prop);                // tambah properti ke daftar milik pemain
    void removeProperty(Property *prop);             // lepas properti dari daftar milik pemain
    const vector<Property *> &getProperties() const; // ambil seluruh properti milik pemain
    int getPropertyCount() const;

    // KARTU
    bool addSkillCard(SkillCard *card);
    void discardSkillCard(int index);
    const vector<SkillCard *> &getHand() const;
    int getHandSize() const;
    void setCardUsedThisTurn(bool used);
    bool hasUsedCardThisTurn() const;

    // JAIL
    void goToJail();
    void releaseFromJail();
    bool isInJail() const;
    void incrementJailTurns();

    // SHIELD CARD
    void activateShield();   // aktifkan efek kebal dari ShieldCard
    bool isShielded() const; // cek apakah pemain sedang terlindungi shield
    void deactivateShield(); // matikan efek shield (dipanggil di akhir giliran)

    // TURN
    void onTurnStart();

    // int getWealth() const;
    // void move(int steps);
    // Property *getPropertyAt(int pos) const { return listProperty[pos]; }
    // int getPropertyNum() const;
    // void showProperty() const;
    // Card *getCardAt(int pos) const { return listCard[pos]; }
    // void addCard(Card *newCard);
    // Card *removeCardAt(int pos);
    // void setStatus(std::string newStatus);
    // std::string getStatus() const { return status; }
    // std::string getId() const { return id; }
};

#endif
