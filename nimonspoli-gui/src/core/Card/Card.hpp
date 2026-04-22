#ifndef CARD_HPP
#define CARD_HPP

#include <string>
using namespace std;

class Player;
class GameMaster;

class Card
{
private:
    string type;
    string description;

public:
    Card();
    Card(const string &type, const string &description);
    virtual ~Card();
    virtual void execute(Player &p, GameMaster &g) = 0;
    string getDescription();
    string getType();
};

#endif