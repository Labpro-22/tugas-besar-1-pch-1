#include "./FestivalCommand.hpp"

FestivalCommand::FestivalCommand(Player *p, TransactionLogger *logger, int turnNumber)
    : p(p), logger(logger), turnNumber(turnNumber) {}

void FestivalCommand::execute(GameMaster&)
{
    // Di mode GUI, execute() hanya mengecek eligibilitas.
    // Jika ada properti eligible, GameScreen akan menampilkan FestivalDialog
    // dan memanggil executeWithProperty(selected) setelah user memilih.
    // Jika tidak ada properti → log saja (tidak ada dialog).
    std::vector<StreetProperty*> streets = getEligibleStreets();
    if (streets.empty()) {
        // Log bahwa efek festival tidak berlaku
        logger->addLog(turnNumber, p->getUsername(), "FESTIVAL",
                       "Tidak ada properti lahan yang memenuhi syarat");
    }
    // Jika ada, GameScreen yang handle (via AWAITING_FESTIVAL phase)
}

std::vector<StreetProperty*> FestivalCommand::getEligibleStreets() const
{
    std::vector<StreetProperty*> streets;
    for (Property *prop : p->getProperties())
    {
        StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
        if (sp && sp->getStatus() != PropertyStatus::MORTGAGED)
            streets.push_back(sp);
    }
    return streets;
}

void FestivalCommand::executeWithProperty(StreetProperty* selected)
{
    if (!selected) return;
    applyFestival(selected);
}

void FestivalCommand::applyFestival(StreetProperty *prop)
{
    int curMult    = prop->getFestivalMultiplier();
    int rentBefore = prop->calculateRentPrice(0, 1, false);

    if (curMult < 8)
    {
        int newMult = (curMult == 1) ? 2 : curMult * 2;
        prop->activateFestival(newMult);

        int rentAfter = prop->calculateRentPrice(0, 1, false);

        logger->addLog(turnNumber, p->getUsername(), "FESTIVAL",
                       prop->getCode() + ": sewa M" + std::to_string(rentBefore) +
                           " -> M" + std::to_string(rentAfter) +
                           ", durasi " + std::to_string(prop->getFestivalDuration()) + " giliran");
    }
    else
    {
        prop->setFestivalDuration(3);

        logger->addLog(turnNumber, p->getUsername(), "FESTIVAL",
                       prop->getCode() + ": efek maks, durasi di-reset 3 giliran");
    }
}