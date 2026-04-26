#pragma once
#include "./Command.hpp"
#include "../Player/Player.hpp"
#include "../Property/StreetProperty.hpp"
#include "../utils/TransactionLogger.hpp"
#include <vector>

class FestivalCommand : public Command
{
private:
    Player *p;
    TransactionLogger *logger;
    int turnNumber;

public:
    FestivalCommand(Player *p, TransactionLogger *logger, int turnNumber);
    void execute(GameMaster& gm) override;

    // GUI API: digunakan oleh FestivalDialog di GameScreen
    std::vector<StreetProperty*> getEligibleStreets() const;
    void executeWithProperty(StreetProperty* selected);
    Player* getPlayer() const { return p; }

private:
    void applyFestival(StreetProperty *prop);
};