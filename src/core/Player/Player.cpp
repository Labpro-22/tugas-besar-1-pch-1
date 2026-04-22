#include "include\models\Player.hpp"

int Player::getWealth() const 
{
    int totalWealth = money;
    for(Property* prop : listProperty) totalWealth += prop->getMortageValue();
    return totalWealth;
}

void Player::move(int steps) 
{
    // nunggu Petak Papan
}

int Player::getPropertyNum() const
{
    return listProperty.size();
}

void Player::showProperty() const
{
    for(Property* prop : listProperty) {
        // do something?
    }
}

void Player::addCard(Card* newCard)
{
    listCard.push_back(newCard);
}

Card* Player::removeCardAt(int pos)
{
    auto iter = listCard.begin(); // udh efficient blm yh?
    while(pos > 0){
        ++iter;
        pos--;
    }
    Card* ret = listCard[pos];
    listCard.erase(iter);
    return ret;
}

void Player::setStatus(std::string newStatus)
{
    status = newStatus;
}
