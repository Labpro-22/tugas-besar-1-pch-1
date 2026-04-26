#include "JualBangunanCommand.hpp"
#include "../Player/Player.hpp"
#include "../Property/StreetProperty.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include <iostream>

JualBangunanCommand::JualBangunanCommand(Player* p, StreetProperty* sp, int turn)
    : player(p), prop(sp), turn(turn) {}

void JualBangunanCommand::execute(GameMaster& gm) {
    if (!player || !prop) return;

    if (prop->getOwnerId() != player->getUsername()) {
        std::cout << "[JualBangunan] Properti bukan milik pemain.\n";
        return;
    }

    if (prop->gethasHotel()) {
        // Jual hotel = kembalikan ke 4 rumah (sesuai spesifikasi monopoly)
        // Dan dapat uang setengah harga hotel
        int sellValue = prop->getHotelUpgCost() / 2;
        prop->downgradeToHouse();
        *player += sellValue;
        gm.log(player->getUsername(), "JUAL_BANGUNAN", "Jual Hotel di " + prop->getName() + " seharga M" + std::to_string(sellValue));
    } else if (prop->getBuildingCount() > 0) {
        // Jual 1 rumah
        int sellValue = prop->getHouseUpgCost() / 2;
        prop->removeBuilding();
        *player += sellValue;
        gm.log(player->getUsername(), "JUAL_BANGUNAN", "Jual 1 Rumah di " + prop->getName() + " seharga M" + std::to_string(sellValue));
    } else {
        std::cout << "[JualBangunan] Tidak ada bangunan untuk dijual.\n";
    }
}
