#ifndef DOCTOR_CARD_HPP
#define DOCTOR_CARD_HPP

#include "GeneralFundCard.hpp"
using namespace std;

class DoctorFeeCard : public GeneralFundCard
{
private:
    double doctorFee;

public:
    DoctorFeeCard();
    DoctorFeeCard(const string &type, const string &description, double doctorFee);
    ~DoctorFeeCard();
    void execute(Player &p, GameMaster &g) override;
};

#endif