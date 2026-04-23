#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "../Property/Property.hpp"

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
    double activeDiscount;
    bool shieldActive;
    string id;

public:
    Player(const std::string &username, int startingBalance);
    virtual ~Player();

    // GET
    void setUsername(const std::string &name) { username = name; }
    string getUsername() const;
    int getBalance() const;
    int getPosition() const;
    PlayerStatus getStatus() const;
    int getJailTurns() const;

    // SET
    void setPosition(int tileIndex);
    void setStatus(PlayerStatus newStatus);
    void setJailTurns(int turns);
    void setBalance(int amount);

    // KEUANGAN
    Player &operator+=(int amount);
    Player &operator-=(int amount);
    bool canAfford(int amount) const;
    bool operator>(const Player &other) const;
    bool operator<(const Player &other) const;

    // PROPERTI
    void addProperty(Property *prop);
    void removeProperty(Property *prop);
    const vector<Property *> &getProperties() const;
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

    // DISKON
    void setDiscount(double percent);
    double getDiscount() const;
    void clearDiscount();

    // SHIELD CARD
    void activateShield();   // aktifkan efek kebal dari ShieldCard
    bool isShielded() const; // cek apakah pemain sedang terlindungi shield
    void deactivateShield(); // matikan efek shield (dipanggil di akhir giliran)

    // TURN
    void onTurnStart();

    int getWealth() const;
    string cetakProperti() const;
};

#endif
