#include "CetakAktaCommand.hpp"

#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"

#include <iostream>
#include <map>
using namespace std;

CetakAktaCommand::CetakAktaCommand(const Property *property)
    : property(property)
{
}

void CetakAktaCommand::execute(GameMaster &gm)
{
    (void)gm;

    if (property == nullptr)
    {
        cout << "Petak tidak ditemukan atau bukan properti.\n";
        return;
    }

    cout << property->cetakAkta();
}