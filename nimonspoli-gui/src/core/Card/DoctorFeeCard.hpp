#ifndef DOCTOR_CARD_HPP
#define DOCTOR_CARD_HPP

#include "GeneralFundCard.hpp"
using namespace std;

class DoctorFeeCard : public GeneralFundCard
{
private:
    int doctorFee;

public:
    DoctorFeeCard();
    DoctorFeeCard(const string &type, const string &description, int amount);
    ~DoctorFeeCard();
    int getDoctorFee() const;
    void execute(Player &p, GameState &gs) override;
};

#endif