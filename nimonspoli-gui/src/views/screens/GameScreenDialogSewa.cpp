#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Player/Player.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Property/RailroadProperty.hpp"
#include "../../core/Property/UtilityProperty.hpp"
#include "../../core/Commands/BayarSewaCommand.hpp"
#include "../../core/GameState/GameState.hpp"
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  triggerSewaDialog
//  Dipanggil dari syncFromGameMaster() saat phase == AWAITING_RENT.
//  Membaca pendingRentTile dari GameState dan menghitung sewa via
//  BayarSewaCommand::calculateRent() (sekarang exposed lewat helper statis).
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::triggerSewaDialog()
{
    if (!isRealMode())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *cur = gs.getCurrPlayer();

    PropertyTile *pt = gs.getPendingRentTile();
    if (!pt || !cur)
        return;

    Property *prop = pt->getProperty();
    if (!prop)
        return;

    // Cari owner
    Player *owner = gs.getPlayerById(prop->getOwnerId());
    if (!owner)
        return;

    // Hitung sewa — buat command sementara, pakai calculateRent yang accessible
    int dice = gs.getPendingRentDice();
    BayarSewaCommand tempCmd(cur, pt, dice, gs.getLogger(), gs.getCurrTurn());
    // calculateRent harus dipanggil dengan GameMaster → buat wrapper inline
    // Kita hitung secara inline sesuai tipe properti agar tidak perlu expose private method

    int rent = 0;
    {
        StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
        if (sp)
        {
            bool monopoly = gm->hasMonopoly(owner, prop->getColorGroup());
            rent = sp->calculateRentPrice(dice, 1, monopoly);
        }
        RailroadProperty *rp = dynamic_cast<RailroadProperty *>(prop);
        if (rp)
        {
            int rrCount = gm->countPlayerRailroads(owner);
            rent = rp->calculateRentPrice(dice, rrCount, false);
        }
        UtilityProperty *up = dynamic_cast<UtilityProperty *>(prop);
        if (up)
        {
            int utilCount = gm->countPlayerUtilities(owner);
            rent = up->calculateRentPrice(dice, utilCount, false);
        }
    }

    // Simpan rent amount ke GameState agar dipakai saat BAYAR ditekan
    gs.setPendingRentAmount(rent);

    // Isi struct dialog
    sewaDialog.tileName = prop->getName() + " (" + prop->getCode() + ")";
    sewaDialog.ownerName = owner->getUsername();
    sewaDialog.rentAmount = rent;
    sewaDialog.landerBalBefore = cur->getBalance();
    sewaDialog.ownerBalBefore = owner->getBalance();
    sewaDialog.canAfford = cur->canAfford(rent);

    // Kondisi bangunan
    StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);
    if (sp)
    {
        if (sp->gethasHotel())
            sewaDialog.conditionStr = "Hotel";
        else if (sp->getBuildingCount() > 0)
            sewaDialog.conditionStr = std::to_string(sp->getBuildingCount()) + " rumah";
        else
            sewaDialog.conditionStr = "Tanah kosong";

        if (sp->getFestivalMultiplier() > 1)
            sewaDialog.conditionStr += " (Festival x" +
                                       std::to_string(sp->getFestivalMultiplier()) + ")";
    }
    else
    {
        sewaDialog.conditionStr = "-";
    }

    sewaDialog.visible = true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawSewaDialog
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::drawSewaDialog()
{
    if (!sewaDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 480.f, PH = 320.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    // Background
    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, {220, 80, 80, 255});

    // Header
    DrawRectangle((int)px, (int)py, (int)PW, 52, {160, 40, 40, 255});
    const char *title = "BAYAR SEWA";
    int tw = MeasureText(title, 18);
    DrawText(title, (int)(px + PW / 2 - tw / 2), (int)(py + 16), 18, WHITE);

    float ry = py + 66;
    float lx = px + 24;
    float rx = px + PW - 24;

    // ── Baris: Lokasi ──────────────────────────────────────────────────────
    DrawText("Lokasi", (int)lx, (int)ry, 12, {140, 140, 180, 255});
    std::string locStr = sewaDialog.tileName + ", milik " + sewaDialog.ownerName + "!";
    int lsw = MeasureText(locStr.c_str(), 13);
    DrawText(locStr.c_str(), (int)(rx - lsw), (int)(ry - 1), 13, {220, 220, 240, 255});
    ry += 26;

    // ── Baris: Kondisi ─────────────────────────────────────────────────────
    DrawText("Kondisi", (int)lx, (int)ry, 12, {140, 140, 180, 255});
    int csw = MeasureText(sewaDialog.conditionStr.c_str(), 13);
    DrawText(sewaDialog.conditionStr.c_str(), (int)(rx - csw), (int)(ry - 1), 13, WHITE);
    ry += 26;

    // ── Divider ────────────────────────────────────────────────────────────
    DrawLine((int)(lx), (int)(ry), (int)(rx), (int)(ry), {60, 60, 90, 255});
    ry += 10;

    // ── Baris: Sewa ────────────────────────────────────────────────────────
    DrawText("Sewa", (int)lx, (int)ry, 12, {140, 140, 180, 255});
    std::string rentStr = "M" + std::to_string(sewaDialog.rentAmount);
    int rsw = MeasureText(rentStr.c_str(), 16);
    DrawText(rentStr.c_str(), (int)(rx - rsw), (int)(ry - 2), 16,
             sewaDialog.canAfford ? Color{255, 200, 60, 255} : Color{255, 80, 80, 255});
    ry += 30;

    // ── Baris: Uang kamu (before → after) ─────────────────────────────────
    int afterLander = sewaDialog.landerBalBefore - sewaDialog.rentAmount;
    DrawText("Uang kamu", (int)lx, (int)ry, 12, {140, 140, 180, 255});
    std::string landerStr = "M" + std::to_string(sewaDialog.landerBalBefore) +
                            " -> M" + std::to_string(afterLander);
    int lw = MeasureText(landerStr.c_str(), 13);
    DrawText(landerStr.c_str(), (int)(rx - lw), (int)(ry - 1), 13,
             sewaDialog.canAfford ? Color{200, 240, 200, 255} : Color{255, 140, 140, 255});
    ry += 26;

    // ── Baris: Uang owner (before → after) ────────────────────────────────
    int afterOwner = sewaDialog.ownerBalBefore + sewaDialog.rentAmount;
    std::string ownerLabel = "Uang " + sewaDialog.ownerName;
    DrawText(ownerLabel.c_str(), (int)lx, (int)ry, 12, {140, 140, 180, 255});
    std::string ownerStr = "M" + std::to_string(sewaDialog.ownerBalBefore) +
                           " -> M" + std::to_string(afterOwner);
    int ow = MeasureText(ownerStr.c_str(), 13);
    DrawText(ownerStr.c_str(), (int)(rx - ow), (int)(ry - 1), 13,
             {100, 220, 140, 255});
    ry += 10;

    // ── Peringatan jika tidak mampu ────────────────────────────────────────
    if (!sewaDialog.canAfford)
    {
        ry += 4;
        const char *warn = "Saldo tidak cukup! Proses kebangkrutan akan dimulai.";
        int ww = MeasureText(warn, 10);
        DrawText(warn, (int)(px + PW / 2 - ww / 2), (int)ry, 10, {255, 120, 80, 255});
        ry += 4;
    }

    // ── Tombol BAYAR ──────────────────────────────────────────────────────
    float btnW = 160.f;
    Rectangle bayarBtn = {px + PW / 2 - btnW / 2, py + PH - 54, btnW, 40};
    bool bayarHov = CheckCollisionPointRec(GetMousePosition(), bayarBtn);
    Color btnColor = bayarHov ? Color{80, 160, 100, 255} : Color{50, 120, 70, 255};
    DrawRectangleRec(bayarBtn, btnColor);
    DrawRectangleLinesEx(bayarBtn, 1.5f, {100, 200, 130, 255});
    const char *btnLabel = sewaDialog.canAfford ? "BAYAR" : "PROSES";
    int blw = MeasureText(btnLabel, 14);
    DrawText(btnLabel, (int)(bayarBtn.x + btnW / 2 - blw / 2), (int)(bayarBtn.y + 12), 14, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bayarHov && isRealMode())
    {
        GameMaster *gm = guiManager->getGameMaster();
        GameState &gs = gm->getState();
        Player *cur = gs.getCurrPlayer();
        PropertyTile *pt = gs.getPendingRentTile();

        if (cur && pt)
        {
            int dice = gs.getPendingRentDice();
            // Push BayarSewaCommand — akan dieksekusi oleh GUIManager::flushCommands()
            guiManager->pushCommand(
                new BayarSewaCommand(cur, pt, dice, gs.getLogger(), gs.getCurrTurn()));
        }

        gs.clearPendingRent();
        sewaDialog.visible = false;
        // Phase dikembalikan oleh BayarSewaCommand::execute() (atau BANKRUPTCY jika gagal)
        gs.setPhase(GamePhase::PLAYER_TURN);
    }
}