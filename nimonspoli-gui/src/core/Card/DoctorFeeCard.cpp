#include "DoctorFeeCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

DoctorFeeCard::DoctorFeeCard() : GeneralFundCard("Biaya dokter. Bayar M700."), doctorFee(700)
{
}

DoctorFeeCard::DoctorFeeCard(const string &type, const string &description, int amount) : GeneralFundCard(description), doctorFee(amount)
{
}

int DoctorFeeCard::getDoctorFee() const
{
    return doctorFee;
}

DoctorFeeCard::~DoctorFeeCard()
{
}

void DoctorFeeCard::execute(Player &p, GameState &gs)
{
    GameMaster* gm = gs.getGameMaster();
    // Bank* bank = gs.getBank();
    
    if (p.getStatus() == PlayerStatus::BANKRUPT) {
        return;
    }

    if (p.getBalance() >= doctorFee) {
        p -= doctorFee;
    } else {
        int status = gm->handleDebtPayment(&p, doctorFee, nullptr);
        if (status == 2) {
            gm->handleBankruptcy(&p, gm->getState().getBank());
        }
    }
}