#include "Player.hpp"
#include "../Property/Property.hpp"
#include "../Card/SkillCard.hpp"
#include <sstream>
#include <map>

Player::Player(const std::string &username, int startingBalance) : username(username),
                                                                   balance(startingBalance),
                                                                   position(0),
                                                                   status(PlayerStatus::ACTIVE),
                                                                   jailTurns(0),
                                                                   cardUsedThisTurn(false),
                                                                   activeDiscount(0.0),
                                                                   shieldActive(false),
                                                                   id(username)
{
}

Player::~Player()
{
}

// GET
string Player::getUsername() const
{
    return username;
}

int Player::getBalance() const
{
    return balance;
}

int Player::getPosition() const
{
    return position;
}

PlayerStatus Player::getStatus() const
{
    return status;
}

int Player::getJailTurns() const
{
    return jailTurns;
}

// SET
void Player::setPosition(int tileIndex)
{
    position = tileIndex;
}

void Player::setStatus(PlayerStatus newStatus)
{
    status = newStatus;
}

void Player::setJailTurns(int turns)
{
    jailTurns = turns;
}

void Player::setBalance(int b)
{
    balance = b;
}

Player &Player::operator+=(int amount)
{
    balance += amount;
    return *this;
}

Player &Player::operator-=(int amount)
{
    balance -= amount;
    return *this;
}

bool Player::canAfford(int amount) const
{
    return balance >= amount;
}

bool Player::operator>(const Player &other) const
{
    return balance > other.balance;
}

bool Player::operator<(const Player &other) const
{
    return balance < other.balance;
}

// PROPERTY
void Player::addProperty(Property *prop)
{
    properties.push_back(prop);
}

void Player::clearProperties()
{
    properties.clear();
}

void Player::removeProperty(Property *prop)
{
    auto it = find(properties.begin(), properties.end(), prop);
    if (it != properties.end())
        properties.erase(it);
}

const vector<Property *> &Player::getProperties() const
{
    return properties;
}

int Player::getPropertyCount() const
{
    return properties.size();
}

// SKILL CARD
bool Player::addSkillCard(SkillCard *card)
{
    if (skillCards.size() >= 3)
        return false;
    skillCards.push_back(card);
    return true;
}

SkillCard *Player::discardSkillCard(int index)
{
    if (index < 0 || index >= static_cast<int>(skillCards.size()))
    {
        return nullptr;
    }

    SkillCard *removed = skillCards[index];
    skillCards.erase(skillCards.begin() + index);
    return removed;
}

const vector<SkillCard *> &Player::getHand() const
{
    return skillCards;
}

int Player::getHandSize() const
{
    return skillCards.size();
}

void Player::markCardUsedThisTurn()
{
    cardUsedThisTurn = true;
}

bool Player::hasUsedCardThisTurn() const
{
    return cardUsedThisTurn;
}

void Player::forceAddSkillCard(SkillCard *card)
{
    if (card == nullptr)
        return;

    skillCards.push_back(card);
}

string Player::printSkillCards() const
{
    ostringstream out;

    out << "Daftar Kartu Kemampuan Anda: \n";
    for (size_t i = 0; i < skillCards.size(); ++i)
    {
        if (skillCards[i] == nullptr)
            continue;

        out << (i + 1) << ". "
            << skillCards[i]->getType()
            << " - "
            << skillCards[i]->getDescription() << "\n";
    }

    return out.str();
}

// JAIL
void Player::goToJail()
{
    status = PlayerStatus::JAILED;
    jailTurns = 0;
}

void Player::releaseFromJail()
{
    status = PlayerStatus::ACTIVE;
    jailTurns = 0;
}

bool Player::isInJail() const
{
    return status == PlayerStatus::JAILED;
}

void Player::incrementJailTurns()
{
    jailTurns++;
}

// DISKON
void Player::setDiscount(double percent)
{
    activeDiscount = percent;
}
double Player::getDiscount() const
{
    return activeDiscount;
}
void Player::clearDiscount()
{
    activeDiscount = 0.0;
}

bool Player::hasActiveDiscount() const
{
    return activeDiscount > 0.0;
}

int Player::applyDiscount(int originalPrice) const
{
    if (originalPrice <= 0)
        return 0;

    if (!hasActiveDiscount())
        return originalPrice;

    int finalPrice = static_cast<int>(
        originalPrice * (100.0 - activeDiscount) / 100.0);

    if (finalPrice < 0)
        return 0;

    return finalPrice;
}

// SHIELD
void Player::activateShield()
{
    shieldActive = true;
}

bool Player::isShielded() const
{
    return shieldActive;
}

void Player::deactivateShield()
{
    shieldActive = false;
}

void Player::onTurnStart()
{
    cardUsedThisTurn = false;
    shieldActive = false;
}

int Player::getWealth() const
{
    int total = balance;
    for (auto *p : properties)
    {
        if(p->getStatus() != PropertyStatus::MORTGAGED) total += p->getPurchasePrice();
    }
    return total;
}

string Player::cetakProperti() const
{
    ostringstream out;

    if (properties.empty())
    {
        out << "Kamu belum memiliki properti apapun.\n";
        return out.str();
    }

    map<string, vector<Property *>> grouped;
    int totalWealth = 0;

    for (Property *prop : properties)
    {
        if (!prop)
            continue;

        grouped[prop->getColorGroup()].push_back(prop);
        totalWealth += prop->calculateSellPrice();
    }

    for (const auto &[group, props] : grouped)
    {
        out << "[" << group << "]\n";
        for (Property *prop : props)
        {
            out << "  - " << prop->printList() << "\n";
        }
        out << "\n";
    }

    out << "Total kekayaan properti: M" << totalWealth << "\n";
    return out.str();
}
