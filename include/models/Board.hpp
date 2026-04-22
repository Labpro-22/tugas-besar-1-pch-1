#pragma once
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
using namespace std;

class Player;
class GameState;
class Bank;
class Dice;
class Card;
class CardDeck;
class Property;
class StreetProperty;
class RailroadProperty;
class UtilityProperty;


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

enum class TileName
{
    GO,
    GARUT,
    COMMON_FUND,
    TASIKMALAYA,
    INCOME_TAX,
    ST_GAMBIR,
    BOGOR,
    FESTIVAL,
    DEPOK,
    BEKASI,
    JAIL,
    MAGELANG,
    PLN,
    SOLO,
    YOGYAKARTA,
    ST_BANDUNG,
    MALANG,
    SEMARANG,
    SURABAYA,
    FREE_PARK,
    MAKASSAR,
    CHANCE,
    BALIKPAPAN,
    MANADO,
    ST_TUGU,
    PALEMBANG,
    PEKANBARU,
    PAM,
    MEDAN,
    GO_TO_JAIL,
    BANDUNG,
    DENPASAR,
    MATARAM,
    ST_GUBENG,
    JAKARTA,
    LUX_GOODS,
    IKN
};

string tileTypeToString(TileType type);
string tileNameToString(TileName name);


class Tile
{
protected:
    int id;
    string colorDisplay;
    TileType type;
    TileName name;
    string code;

public:
    Tile(int id, string display, TileType type, TileName name, string code);
    virtual void onLanded(Player &p, GameState &gs) = 0;
    virtual ~Tile() = default;
    int getIndex() const;
    string getCode() const;
    string getTileName() const;
    string getTileType() const;
    string getDisp() const;
};


class Board
{
private:
    vector<Tile *> tiles; 
    int size;

public:
    Board(const vector<Tile *> &tiles, int size);
    ~Board(); 
    Tile *getTile(int idx) const;
    int getNextTileIndex(int cur, int steps) const;
    Tile *getNextTile(int cur, int steps) const;
    int getSize() const;
    int findTileIndexByCode(const string &code) const;
    Tile *findTileByCode(const string &code) const;
};

class ActionTile : public Tile
{
public:
    ActionTile(int id, string display, TileType type, TileName name, string code);
};

class PropertyTile : public Tile
{
protected:
    Property *prop;

public:
    PropertyTile(int id, string display, TileType type, TileName name, string code, Property *prop);
    Property *getProperty() const;
    virtual int calculateRent(int diceTotal) const = 0;
    void onLanded(Player &p, GameState &gs) override;

protected:
    virtual void handleUnowned(Player &p, GameState &gs) = 0;
    virtual void handleOwnedByOther(Player &p, GameState &gs) = 0;
};


class StreetTile : public PropertyTile
{
public:
    StreetTile(int id, string display, TileType type, TileName name, string code, Property *prop);
    int calculateRent(int diceTotal) const override;

protected:
    void handleUnowned(Player &p, GameState &gs) override;
    void handleOwnedByOther(Player &p, GameState &gs) override;
};

class RailroadTile : public PropertyTile
{
public:
    RailroadTile(int id, string display, TileType type, TileName name, string code, Property *prop);
    int calculateRent(int diceTotal) const override;

protected:
    void handleUnowned(Player &p, GameState &gs) override;
    void handleOwnedByOther(Player &p, GameState &gs) override;
};

class UtilityTile : public PropertyTile
{
public:
    UtilityTile(int id, string display, TileType type, TileName name, string code, Property *prop);
    int calculateRent(int diceTotal) const override;

protected:
    void handleUnowned(Player &p, GameState &gs) override;
    void handleOwnedByOther(Player &p, GameState &gs) override;
};


class GoTile : public ActionTile
{
private:
    int salary;

public:
    GoTile(int id, string display, TileType type, TileName name, string code, int salary);
    void onLanded(Player &p, GameState &gs) override;
    void onPassed(Player &p, GameState &gs);
    int getSalary() const;
};

class JailTile : public ActionTile
{
private:
    vector<Player *> inmates;
    vector<Player *> visitors;
    int jailFine;
    int jailIndex; 
public:
    JailTile(int id, string display, TileType type, TileName name, string code, int jailFine);
    int getJailFine() const;
    void onLanded(Player &p, GameState &gs) override;
    void sendToJail(Player &p);
    bool tryEscape(Player &p, Dice &d);
    void payFine(Player &p, Bank &b);
    void useJailCard(Player &p); 
    void release(Player &p);
    bool isInmate(const Player &p) const;
};

class GoToJailTile : public ActionTile
{
public:
    GoToJailTile(int id, string display, TileType type, TileName name, string code);
    void onLanded(Player &p, GameState &gs) override;
};

class FreeParkingTile : public ActionTile
{
public:
    FreeParkingTile(int id, string display, TileType type, TileName name, string code);
    void onLanded(Player &p, GameState &gs) override;
};

class TaxTile : public ActionTile
{
public:
    TaxTile(int id, string display, TileType type, TileName name, string code);
    void onLanded(Player &p, GameState &gs) override;
};

class CardTile : public ActionTile
{
private:
    CardDeck *deck; 
public:
    CardTile(int id, string display, TileType type, TileName name, string code, CardDeck *deck);
    void onLanded(Player &p, GameState &gs) override;
};

class FestivalTile : public ActionTile
{
public:
    FestivalTile(int id, string display, TileType type, TileName name, string code);
    void onLanded(Player &p, GameState &gs) override;
    // Implementasi di sini untuk onLanded
};