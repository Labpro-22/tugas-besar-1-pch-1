#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"
using namespace std;

class Property;

class DemolitionCard : public SkillCard
{
private:
    int targetPropertyId;

public:
    DemolitionCard();
    DemolitionCard(const string &type, const string &description, bool used);
    ~DemolitionCard();
    int getTargetPropertyId() const;
    void setTargetProperty(int targetPropertyId);
    void execute(Player &p, GameState &gs) override;
    string successMessage() const override;
};

#endif