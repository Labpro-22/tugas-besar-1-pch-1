#include "GunakanKemampuanCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"
#include "../Card/SkillCard.hpp"

#include <iostream>
#include <limits>

using namespace std;

void GunakanKemampuanCommand::execute(GameMaster &gm)
{
    Player *player = gm.getState().getCurrPlayer();
    if (player == nullptr)
    {
        cout << "Tidak ada pemain aktif.\n";
        return;
    }

    if (gm.getState().getHasRolled())
    {
        cout << "Kartu kemampuan hanya bisa digunakan SEBELUM melempar dadu.\n";
        return;
    }

    if (player->hasUsedCardThisTurn())
    {
        cout << "Kamu sudah menggunakan kartu kemampuan pada giliran ini! "
             << "Penggunaan kartu dibatasi maksimal 1 kali dalam 1 giliran.\n";
        return;
    }

    const vector<SkillCard *> &hand = player->getHand();
    if (hand.empty())
    {
        cout << "Kamu tidak memiliki kartu kemampuan.\n";
        return;
    }

    cout << "Daftar Kartu Kemampuan Spesial Anda:\n";
    for (size_t i = 0; i < hand.size(); ++i)
    {
        if (hand[i] == nullptr)
            continue;

        cout << (i + 1) << ". "
             << hand[i]->getType() << " - "
             << hand[i]->getDescription() << "\n";
    }
    cout << "0. Batal\n\n";

    int pilihan;
    while (true)
    {
        cout << "Pilih kartu yang ingin digunakan (0-" << hand.size() << "): ";

        if (!(cin >> pilihan))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid. Masukkan angka.\n";
            continue;
        }

        if (pilihan < 0 || pilihan > (int)hand.size())
        {
            cout << "Pilihan di luar rentang.\n";
            continue;
        }

        break;
    }

    if (pilihan == 0)
    {
        cout << "Penggunaan kartu dibatalkan.\n";
        return;
    }

    SkillCard *card = hand[pilihan - 1];
    if (card == nullptr)
    {
        cout << "Kartu tidak valid.\n";
        return;
    }

    gm.useSkillCard(player, card, gm.getState());
}