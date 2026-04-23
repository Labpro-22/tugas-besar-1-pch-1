#include "DoctorFeeCard.hpp"
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"

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
    p -= doctorFee;
}