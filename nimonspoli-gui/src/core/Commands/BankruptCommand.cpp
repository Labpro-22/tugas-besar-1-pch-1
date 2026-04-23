#include "../Commands/BankruptCommand.hpp"

BankruptCommand::BankruptCommand(GameMaster& gm, GameState& gs, Player* from, Player* to, int debt, bool sellOverMortgage, int propToSell = -1) 
    : gm(gm), state(gs), from(from), to(to), debt(debt), sellOverMortgage(sellOverMortgage), propToSell(propToSell) {}

// While debt is unpaid (and can be paid with liquidation), try this
void BankruptCommand::execute(GameMaster& gm)
{
    GameState& gs = gm.getState();
    // Player* currPlayer = gs.getCurrPlayer();
    int liquidation = gm.handleDebtPayment(from, debt, to);

    std::cout << "Evaluasi Bankruptcy" << std::endl;
    if (liquidation == 0)
    {
        // Handle Transfer
        std::cout << "Melakukan Transfer Balance ke ";
        if (to)
        {
            *from -= debt;
            *to += debt;
            std::cout << to->getUsername() << std::endl;
        }
        else
        {
            *from -= debt;
            std::cout << "Bank" << std::endl;
        }
    }
    else if (liquidation == 1) 
    {
        while (from->getBalance() < debt) {
            // Belum ada guardnyaaaaaaaaa
            std::cout << "Prop idx: ";
            std::cin >> propToSell;
            std::cout << "State: ";
            std::cin >> sellOverMortgage;
            if (propToSell < 0 || propToSell >= from->getPropertyCount()) {
                // err idx out of bounds
                continue;
            }
            Property* prop = from->getProperties()[propToSell];
            if (sellOverMortgage)
            {
                std::cout << "Menjual properti " << prop->getName() << std::endl;
                gm.sellPropertyToBank(from, prop);
            }
            else
            {
                std::cout << "Menggadaikan properti " << prop->getName() << std::endl;
                gm.mortgageProperty(from, prop);
            }
        }
         
    }
    else if (liquidation == 2)
    {
        std::cout << "Player " << from->getUsername() << " telah BANKRUPT";
        if (to) 
        {
            gm.handleBankruptcy(from, to);
            std::cout << "ke " << to->getUsername() << std::endl;
        }
        else {
            gm.handleBankruptcy(from, gs.getBank());
            std::cout << "ke Bank" << std::endl;
        }
    }
}