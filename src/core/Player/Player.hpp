#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <string>
#include <memory>
#include "../Card/Card.hpp"
#include "../Property/Property.hpp"
#include "../Board/Board.hpp"

class Player {
private: 
    std::string id;
    int money;
    Tile* currPetak; // nanti ganti
    std::vector<Property*> listProperty;
    std::vector<Card*> listCard;
    std::string status;
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
    Player(std::string id, int money, Tile* currPetak, std::vector<Property*> listProperty, std::vector<Card*> listCard, std::string status) 
        : id(id), money(money), currPetak(currPetak), listProperty(listProperty), listCard(listCard), status(status) {}
    
    std::string getID() const {return id;}
    int getMoney() const {return money;}

    Player& operator=(int amount) {
        money = amount;
        return *this;
    }
    Player& operator+(int amount){
        money += amount;
        return *this;
    }
    Player& operator-(int amount){
        money -= amount;
        return *this;
    }

    int getWealth() const;
    void move(int steps);
    Property* getPropertyAt(int pos) const {return listProperty[pos];}
    int getPropertyNum() const;
    void showProperty() const;
    Card* getCardAt(int pos) const {return listCard[pos];}
    void addCard(Card* newCard);
    Card* removeCardAt(int pos);
    void setStatus(std::string newStatus);
    std::string getStatus() const {return status;}
    std::string getId() const{return id;}

    };

#endif
