#pragma once
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "../Property/Property.hpp"
#include "../Player/Player.hpp"
#include "../Dice/Dice.hpp"
#include "../Bank/Bank.hpp"
#include "../GameState/GameState.hpp"
#include "../Card/CardDeck.hpp"
#include "../Card/Card.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Commands/BayarPajakCommand.hpp"
#include "../Commands/FestivalCommand.hpp"
#include "../Commands/BangunCommand.hpp"
#include "../Commands/BayarSewaCommand.hpp"
#include "../Commands/CardCommand.hpp"
#include "../GameState/GameState.hpp"
#include "../Exceptions/FinancialException.hpp"
#include "../GameMaster/GameMaster.hpp"

using namespace std;

// ─────────────────────────────────────────────
//  Forward declarations
// ─────────────────────────────────────────────
class GameState;

// ─────────────────────────────────────────────
//  Enums (harus sebelum Tile & Board)
// ─────────────────────────────────────────────

enum class TileType
{
    SPECIAL,
    STREET,
    CARD,
    TAX,
    RAILROAD,
    FESTIVAL,
    UTILITY
};

string tileTypeToString(TileType type);

// ─────────────────────────────────────────────
//  Tile (harus sebelum Board)
// ─────────────────────────────────────────────

class Tile
{
protected:
    int id;
    string colorDisplay;
    TileType type;
    string name;
    string code;
    string picDisplay;

public:
    Tile(int id, string display, TileType type, string name, string code);
    virtual void onLanded(Player &p, GameState &gs) = 0;
    virtual ~Tile() = default;
    int getIndex() const;
    string getCode() const;
    string getTileName() const;
    string getTileType() const;
    string getDisp() const;
    string getPic() const;
};

// ─────────────────────────────────────────────
//  Board (setelah Tile)
// ─────────────────────────────────────────────

class Board
{
private:
    vector<Tile *> tile;
    int size;

public:
    Board(const vector<Tile *> &tiles, int size);
    ~Board();
    Tile *getTile(int idx) const;
    Tile *getNextTile(int cur, int steps) const;
    int getSize() const;
    int findTileIndexByCode(const string &code) const;
    Tile *findTileByCode(const string &code) const;
    Property *findPropertyById(int id) const;
};

// ─────────────────────────────────────────────
//  ActionTile
// ─────────────────────────────────────────────

class ActionTile : public Tile
{
public:
    ActionTile(int id, string display, TileType type, string name, string code);
};

// ─────────────────────────────────────────────
//  PropertyTile
// ─────────────────────────────────────────────

class PropertyTile : public Tile
{
protected:
    Property *prop;

public:
    PropertyTile(int id, string display, TileType type, string name, string code);
    Property *getProperty() const;
    virtual int calculateRent(int diceTotal) const = 0;
    void setProperty(Property *p) { prop = p; }
};

class StreetTile : public PropertyTile
{
public:
    StreetTile(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
    int calculateRent(int diceTotal) const override;
};

class RailRoadTile : public PropertyTile
{
public:
    RailRoadTile(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
    int calculateRent(int diceTotal) const override;
};

class UtilityTile : public PropertyTile
{
public:
    UtilityTile(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
    int calculateRent(int diceTotal) const override;
};

// ─────────────────────────────────────────────
//  Action tiles
// ─────────────────────────────────────────────

class GoTile : public ActionTile
{
private:
    int salary;

public:
    GoTile(int id, string display, TileType type, string name, string code, int salary);
    void onLanded(Player &, GameState &) override;
    void onPassed(Player &, GameState &);
    int getSalary() const;
};

class JailTile : public ActionTile
{
private:
    vector<Player *> inmates;
    vector<Player *> visitor;
    int jailFine;

public:
    JailTile(int id, string display, TileType type, string name, string code,
             vector<Player *> inmates, vector<Player *> visitor, int jailFine);
    int getJailFine() const;
    void onLanded(Player &, GameState &) override;
    void sendToJail(Player &);
    bool tryEscape(Player &, Dice &);
    void payFine(Player &, GameState &);
    void useJailCard(Player &);
    void release(Player &);
    bool isInmate(Player &) const;
};

class GoToJail : public ActionTile
{
public:
    GoToJail(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
};

class FreeParkingTile : public ActionTile
{
public:
    FreeParkingTile(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
};

class TaxTile : public ActionTile
{
public:
    TaxTile(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
};

class CardTile : public ActionTile
{
private:
    CardDeck<Card> *card;

public:
    CardTile(int id, string display, TileType type, string name, string code, CardDeck<Card> *deck);
    void onLanded(Player &, GameState &) override;
};

class FestivalTile : public ActionTile
{
public:
    FestivalTile(int id, string display, TileType type, string name, string code);
    void onLanded(Player &, GameState &) override;
};