#include "DoctorFeeCard.hpp"

DoctorFeeCard::DoctorFeeCard()
{
}

DoctorFeeCard::DoctorFeeCard(const string &type, const string &description, double doctorFee) : GeneralFundCard(type, description), doctorFee(doctorFee)
{
}

DoctorFeeCard::~DoctorFeeCard()
{
}

void DoctorFeeCard::execute(Player &p, GameMaster &g)
{
}