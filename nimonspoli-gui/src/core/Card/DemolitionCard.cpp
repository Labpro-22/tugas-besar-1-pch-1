#include "DemolitionCard.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../Board/Board.hpp"
#include "../Player/Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include <stdexcept>

DemolitionCard::DemolitionCard()
    : SkillCard("Hancurkan properti milik lawan", "DemolitionCard"),
      targetPropertyId(-1)
{
}

DemolitionCard::DemolitionCard(const string &type, const string &description, bool used)
    : SkillCard(type, description, used),
      targetPropertyId(-1)
{
}

DemolitionCard::~DemolitionCard()
{
}

int DemolitionCard::getTargetPropertyId() const
{
    return targetPropertyId;
}

void DemolitionCard::setTargetProperty(int targetPropertyId)
{
    this->targetPropertyId = targetPropertyId;
}

void DemolitionCard::execute(Player &p, GameState &gs)
{
    if (targetPropertyId == -1)
        throw runtime_error("Target properti DemolitionCard belum dipilih.");

    GameMaster *gm = gs.getGameMaster();
    if (!gm)
        throw runtime_error("GameMaster tidak tersedia.");

    Board *board = gm->getState().getBoard();
    if (!board)
        throw runtime_error("Board tidak tersedia.");

    Property *targetProperty = board->findPropertyById(targetPropertyId);
    if (!targetProperty)
        throw runtime_error("Properti target tidak ditemukan.");

    if (targetProperty->getOwnerId() == p.getUsername())
        throw runtime_error("DemolitionCard tidak bisa digunakan pada properti sendiri.");

    if (targetProperty->getStatus() != PropertyStatus::OWNED)
        throw runtime_error("DemolitionCard hanya bisa digunakan pada properti milik lawan yang berstatus OWNED.");

    StreetProperty *sp = dynamic_cast<StreetProperty *>(targetProperty);
    if (!sp)
        throw runtime_error("DemolitionCard hanya bisa digunakan pada properti Street.");

    if (sp->getBuildingCount() <= 0 && !sp->gethasHotel())
        throw runtime_error("Properti target tidak memiliki bangunan yang bisa dihancurkan.");

    sp->sellAllBuildings();

    targetPropertyId = -1;
    markUsed();
}

string DemolitionCard::successMessage() const
{
    return "DemolitionCard berhasil digunakan.";
}