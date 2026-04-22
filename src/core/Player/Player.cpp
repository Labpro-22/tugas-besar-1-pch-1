#include "Player.hpp"

Player::Player(const std::string &username, int startingBalance) : username(username), balance(startingBalance)
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

void Player::addProperty(Property *prop)
{
    properties.push_back(prop);
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
