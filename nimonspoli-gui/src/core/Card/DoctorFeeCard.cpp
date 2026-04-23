#include "DoctorFeeCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

DoctorFeeCard::DoctorFeeCard() : GeneralFundCard("Biaya dokter. Bayar M700."), doctorFee(700)
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
    // Cek apakah player akan Bankrupt? <-- belum dihandle
    GameMaster* gm = gs.getGameMaster();
    Bank* bank = gs.getBank();
    if (p.getBalance() < doctorFee) {
        // p.setStatus(PlayerStatus::BANKRUPT);
        // Handle bankrupt
        if (p.getStatus() == PlayerStatus::BANKRUPT) {
            return;
        }
        // Handle Bankrupt?
        if (p.getBalance() < 700) {
            gm->handleBankruptcy(&p, bank);
        }
        else p -= doctorFee;
    }
    // p -= doctorFee;
}