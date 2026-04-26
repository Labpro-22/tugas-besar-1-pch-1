#include "GameScreen.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Commands/BangunCommand.hpp"

void GameScreen::triggerBangunDialog()
{
    bangunDialog.visible = true;
    bangunDialog.scrollY = 0.f;
    bangunDialog.selectedGroupIdx = -1;
    bangunDialog.groups.clear();
    bangunDialog.hoveredIdx = -1;

    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    Player *cur = gm->getState().getCurrPlayer();
    Board *board = gm->getState().getBoard();
    if (!cur || !board)
        return;

    std::map<std::string, std::vector<StreetProperty *>> ownerGroups;
    for (int i = 0; i < board->getSize(); ++i)
    {
        auto *pt = dynamic_cast<PropertyTile *>(board->getTile(i));
        if (pt && pt->getProperty())
        {
            if (auto *sp = dynamic_cast<StreetProperty *>(pt->getProperty()))
            {
                if (sp->getOwnerId() == cur->getUsername())
                {
                    ownerGroups[sp->getColorGroup()].push_back(sp);
                }
            }
        }
    }

    for (auto const &[color, props] : ownerGroups)
    {
        if (!gm->hasMonopoly(cur, color))
            continue;
        bool anyMortgaged = false;
        for (auto *sp : props)
            if (sp->getStatus() == PropertyStatus::MORTGAGED)
            {
                anyMortgaged = true;
                break;
            }
        if (anyMortgaged)
            continue;
        bangunDialog.groups.push_back({color, props});
    }
}

void GameScreen::drawBangunDialog()
{
    if (!bangunDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 170});

    float PW = 600.f, PH = 500.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 1.5f, {80, 80, 130, 255});
    DrawRectangle((int)px, (int)py, (int)PW, 44, {32, 34, 52, 255});
    DrawText("BELI BANGUNAN", (int)(px + 16), (int)(py + 14), 16, {180, 180, 230, 255});

    Rectangle xBtn = {px + PW - 36, py + 8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);
    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
        bangunDialog.visible = false;

    if (bangunDialog.groups.empty())
    {
        DrawText("Kamu belum memiliki monopoli grup warna apapun.", (int)px + 20, (int)py + 80, 14, {150, 150, 150, 255});
        return;
    }

    float listY = py + 60;

    if (bangunDialog.selectedGroupIdx == -1)
    {
        DrawText("Pilih Color Group:", (int)px + 20, (int)listY, 14, {200, 200, 200, 255});
        listY += 30;
        for (size_t i = 0; i < bangunDialog.groups.size(); ++i)
        {
            float rowY = listY + i * 40;
            if (rowY + 40 > py + PH)
                break;
            Rectangle row = {px + 10, rowY, PW - 20, 36};
            bool hover = CheckCollisionPointRec(GetMousePosition(), row);
            DrawRectangleRec(row, hover ? Color{40, 45, 65, 255} : Color{30, 32, 48, 255});
            DrawText(bangunDialog.groups[i].first.c_str(), (int)row.x + 10, (int)row.y + 10, 14, WHITE);
            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                bangunDialog.selectedGroupIdx = i;
            }
        }
    }
    else
    {
        auto &group = bangunDialog.groups[bangunDialog.selectedGroupIdx];
        std::string title = "Grup: " + group.first;
        DrawText(title.c_str(), (int)px + 20, (int)listY, 14, {200, 200, 200, 255});
        listY += 30;

        // Ambil balance player
        int playerBalance = 0;
        if (isRealMode())
        {
            Player *cur = guiManager->getGameMaster()->getState().getCurrPlayer();
            if (cur)
                playerBalance = cur->getBalance();
        }

        // Cek apakah semua siap upgrade hotel
        auto &groupEntries = group.second;
        bool allFour = true;
        for (auto *ep : groupEntries)
            if (!ep->gethasHotel() && ep->getBuildingCount() < 4)
            {
                allFour = false;
                break;
            }

        float rowY = listY;
        for (size_t i = 0; i < group.second.size(); ++i)
        {
            if (rowY + 40 > py + PH)
                break;

            auto *sp = group.second[i];
            bool isHotel = allFour && sp->getBuildingCount() == 4;
            int cost = isHotel ? sp->getHotelUpgCost() : sp->getHouseUpgCost();
            bool hasHotel = sp->gethasHotel();
            bool canAfford = (playerBalance >= cost);

            Rectangle row = {px + 10, rowY, PW - 20, 36};
            bool hover = CheckCollisionPointRec(GetMousePosition(), row);

            // Warna background: gelap jika sudah hotel atau tidak mampu
            Color rowBg;
            if (hasHotel)
                rowBg = {25, 25, 30, 255};
            else if (!canAfford)
                rowBg = {35, 20, 20, 255}; // kemerahan gelap
            else
                rowBg = hover ? Color{40, 45, 65, 255} : Color{30, 32, 48, 255};

            DrawRectangleRec(row, rowBg);

            // Teks utama
            std::string statusStr = hasHotel ? "HOTEL" : (std::to_string(sp->getBuildingCount()) + " Rumah");
            std::string txt = sp->getName() + " - " + statusStr + " - Biaya: M" + std::to_string(cost);

            Color txtColor;
            if (hasHotel)
                txtColor = {80, 80, 80, 255};
            else if (!canAfford)
                txtColor = {140, 100, 100, 255}; // abu kemerahan
            else
                txtColor = WHITE;

            DrawText(txt.c_str(), (int)row.x + 10, (int)row.y + 10, 14, txtColor);

            // Label keterangan di sisi kanan
            if (hasHotel)
            {
                const char *lbl = "Sudah maksimal";
                int lblW = MeasureText(lbl, 11);
                DrawText(lbl, (int)(row.x + PW - 20 - lblW - 10), (int)(row.y + 12), 11, {80, 80, 80, 255});
            }
            else if (!canAfford)
            {
                const char *lbl = "Uang tidak cukup";
                int lblW = MeasureText(lbl, 11);
                DrawText(lbl, (int)(row.x + PW - 20 - lblW - 10), (int)(row.y + 12), 11, {200, 70, 70, 255});
            }

            // Klik hanya jika bisa dibangun
            if (!hasHotel && canAfford && hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (isRealMode())
                {
                    GameMaster *gm = guiManager->getGameMaster();
                    GameState &gs = gm->getState();
                    Player *cur = gs.getCurrPlayer();
                    if (cur)
                    {
                        guiManager->pushCommand(
                            new BangunCommand(cur, sp, isHotel, gs.getLogger(), gs.getCurrTurn()));
                    }
                }
                bangunDialog.visible = false;
            }

            rowY += 40;
        }

        // Tombol kembali
        Rectangle backBtn = {px + 20, py + PH - 50, 100, 30};
        bool backHov = CheckCollisionPointRec(GetMousePosition(), backBtn);
        DrawRectangleRec(backBtn, backHov ? Color{60, 60, 80, 255} : Color{40, 40, 60, 255});
        DrawText("KEMBALI", (int)backBtn.x + 20, (int)backBtn.y + 8, 14, WHITE);
        if (backHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            bangunDialog.selectedGroupIdx = -1;
        }
    }
}