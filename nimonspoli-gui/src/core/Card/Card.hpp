#ifndef CARD_HPP
#define CARD_HPP

#include <string>
using namespace std;

class Player;
class GameState;

class Card
{
private:
    string description;

public:
    Card();
    Card(const string &description);
    virtual ~Card();
    virtual void execute(Player &p, GameState &gs) = 0;
    string getDescription();
};

#endif