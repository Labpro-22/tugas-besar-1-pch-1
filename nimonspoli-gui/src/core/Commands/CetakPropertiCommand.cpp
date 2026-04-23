#include "CetakPropertiCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"

#include <iostream>
using namespace std;

void CetakPropertiCommand::execute(GameMaster &gm)
{
    Player *player = gm.getState().getCurrPlayer();

    if (player == nullptr)
    {
        cout << "Tidak ada pemain aktif.\n";
        return;
    }

    cout << player->cetakProperti();
}