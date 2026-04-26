#include "CetakAktaCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Board/Board.hpp"
#include "../Property/Property.hpp"
#include "../Player/Player.hpp"

#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

static string toUpperString(string text)
{
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char c)
              {
                  return static_cast<char>(toupper(c));
              });

    return text;
}

CetakAktaCommand::CetakAktaCommand(const string &kodePetak)
    : kodePetak(toUpperString(kodePetak))
{
}

string CetakAktaCommand::getOutput(GameMaster &gm)
{
    if (kodePetak.empty())
    {
        return "Masukkan kode petak.\n";
    }

    Property *property = gm.getState().getPropertyByCode(kodePetak);

    if (!property)
    {
        return "Petak \"" + kodePetak + "\" tidak ditemukan atau bukan properti.\n";
    }

    return property->cetakAkta();
}

void CetakAktaCommand::execute(GameMaster &gm)
{
    cout << getOutput(gm);
}