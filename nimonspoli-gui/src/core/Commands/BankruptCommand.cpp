#include "../Commands/BankruptCommand.hpp"

BankruptCommand::BankruptCommand(GameMaster& gm, GameState& gs, Player* from, Player* to, int debt, bool sellOverMortgage, int propToSell)
    : gm(gm), state(gs), from(from), to(to), debt(debt), sellOverMortgage(sellOverMortgage), propToSell(propToSell) {}

// Dispatcher: evaluasi hutang, simpan konteks ke GameState, atau bangkrutkan langsung.
// Pilihan properti untuk likuidasi dilakukan oleh GUI (BankruptcyDialog).
void BankruptCommand::execute(GameMaster& gm)
{
    GameState& gs = gm.getState();

    // Kasus 0: cash cukup → bayar langsung
    if (from->getBalance() >= debt)
    {
        if (to) { *from -= debt; *to += debt; }
        else    { *from -= debt; }
        gs.setPhase(GamePhase::PLAYER_TURN);
        return;
    }

    int liquidation = gm.handleDebtPayment(from, debt, to);
    // handleDebtPayment sudah set fase BANKRUPTCY dan simpan pendingDebt/Creditor

    if (liquidation == 1)
    {
        // Fase BANKRUPTCY sudah di-set oleh handleDebtPayment.
        // GUI (BankruptcyDialog) yang akan handle pilihan likuidasi.
        std::cout << "[BankruptCommand] Menunggu likuidasi GUI untuk M"
                  << debt << std::endl;
    }
    else if (liquidation == 2)
    {
        // Tidak bisa bayar sama sekali → langsung bangkrut
        std::cout << "Player " << from->getUsername() << " telah BANKRUPT ke ";
        if (to)
        {
            gm.handleBankruptcy(from, to);
            std::cout << to->getUsername() << std::endl;
        }
        else
        {
            gm.handleBankruptcy(from, gs.getBank());
            std::cout << "Bank" << std::endl;
        }
    }
}