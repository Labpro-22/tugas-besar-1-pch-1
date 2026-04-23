#include "CetakAktaCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Property/Property.hpp"

#include <iostream>
using namespace std;

CetakAktaCommand::CetakAktaCommand(const string &kodePetak)
    : kodePetak(kodePetak)
{
}

void CetakAktaCommand::execute(GameMaster &gm)
{
    Property *property = gm.getState().getPropertyByCode(kodePetak);

    if (!property)
    {
        cout << "Petak \"" << kodePetak << "\" tidak ditemukan atau bukan properti.\n";
        return;
    }

    cout << property->cetakAkta();
}