#include "DemolitionCard.hpp"
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"

DemolitionCard::DemolitionCard() : SkillCard("Hancurkan properti milik lawan", "DemolitionCard")
{
}

DemolitionCard::~DemolitionCard()
{
}

void DemolitionCard::execute(Player &p, GameState &gs)
{
    vector<Player *> players = gs.getActivePlayers();

    for (Player *other : players)
    {
        if (other == &p)
            continue;
        vector<Property *> props = other->getProperties();
        for (Property *prop : props)
        {
            StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
            if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel()))
            {
                sp->sellAllBuildings();
                break;
            }
        }
        break;
    }
    markUsed();
}
