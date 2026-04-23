#include "DropKemampuanCommand.hpp"
#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"
#include "../Card/SkillCard.hpp"

#include <iostream>
#include <limits>

using namespace std;

void DropKartuKemampuanCommand::execute(GameMaster &gm)
{
    Player *player = gm.getState().getCurrPlayer();

    if (player == nullptr)
    {
        cout << "Tidak ada pemain aktif.\n";
        return;
    }

    if (player->getHandSize() <= 3)
    {
        return;
    }

    cout << "PERINGATAN: Kamu sudah memiliki "
         << player->getHandSize()
         << " kartu di tangan (Maksimal 3)! "
         << "Kamu diwajibkan membuang 1 kartu.\n\n";

    cout << player->printSkillCards() << "\n";

    int pilihan;
    while (true)
    {
        cout << "Pilih nomor kartu yang ingin dibuang (1-"
             << player->getHandSize() << "): ";

        if (!(cin >> pilihan))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid. Masukkan angka.\n";
            continue;
        }

        if (pilihan < 1 || pilihan > player->getHandSize())
        {
            cout << "Pilihan di luar rentang.\n";
            continue;
        }

        break;
    }

    SkillCard *kartuDibuang = player->getHand()[pilihan - 1];
    string namaKartu = (kartuDibuang != nullptr) ? kartuDibuang->getType() : "Kartu";

    player->discardSkillCard(pilihan - 1);

    cout << "\n"
         << namaKartu << " telah dibuang. "
         << "Sekarang kamu memiliki "
         << player->getHandSize()
         << " kartu di tangan.\n";
}