#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Commands/LemparDaduCommand.hpp"
#include "../../core/Card/FreeFromJailCard.hpp"
#include <string>

// ─────────────────────────────────────────────
//  triggerJailDialog()
//  Dipanggil dari syncDiceResult() saat phase == AWAITING_JAIL
// ─────────────────────────────────────────────
static int findFreeJailCardIdx(Player *p)
{
    if (!p)
        return -1;
    const auto &hand = p->getHand();
    for (int i = 0; i < (int)hand.size(); i++)
    {
        if (dynamic_cast<FreeFromJailCard *>(hand[i]))
            return i;
    }
    return -1;
}

void GameScreen::triggerJailDialog()
{
    if (!guiManager || !guiManager->getGameMaster())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *player = gs.getCurrPlayer();
    if (!player || !player->isInJail())
        return;

    // Ambil jailFine dari config via JailTile
    int fine = 50; // fallback default jika JailTile tidak ditemukan
    Board *board = gs.getBoard();
    if (board)
    {
        for (int i = 0; i < board->getSize(); i++)
        {
            JailTile *jt = dynamic_cast<JailTile *>(board->getTile(i));
            if (jt)
            {
                fine = jt->getJailFine();
                break;
            }
        }
    }

    int turns = player->getJailTurns(); // sudah berapa giliran di penjara

    jailDialog.visible = true;
    jailDialog.jailFine = fine;
    jailDialog.canAffordFine = player->canAfford(fine);
    jailDialog.jailTurnsLeft = turns;
    jailDialog.forcedPay = (turns >= 3); // giliran ke-4 wajib bayar
    jailDialog.rolledThisTurn = false;
}

// ─────────────────────────────────────────────
//  drawJailDialog()
// ─────────────────────────────────────────────
void GameScreen::drawJailDialog()
{
    if (!jailDialog.visible)
        return;

    // ── Overlay ───────────────────────────────────────────────────────────
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 170});

    float pw = 460.f, ph = 340.f;
    float px = SCREEN_W / 2.f - pw / 2.f;
    float py = SCREEN_H / 2.f - ph / 2.f;

    DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {25, 27, 38, 255});
    DrawRectangleLinesEx({px, py, pw, ph}, 2.f, {180, 80, 80, 255});

    // Header
    DrawRectangle((int)px, (int)py, (int)pw, 52, {120, 40, 40, 255});
    const char *title = "DALAM PENJARA";
    int tw = MeasureText(title, 18);
    DrawText(title, (int)(px + pw / 2 - tw / 2), (int)(py + 8), 18, WHITE);

    // Subtitle giliran
    std::string sub = "Giliran ke-" + std::to_string(jailDialog.jailTurnsLeft + 1) + " di penjara";
    if (jailDialog.forcedPay)
        sub += "  [WAJIB BAYAR DENDA]";
    int sw = MeasureText(sub.c_str(), 11);
    DrawText(sub.c_str(), (int)(px + pw / 2 - sw / 2), (int)(py + 33), 11,
             jailDialog.forcedPay ? Color{255, 120, 80, 255} : Color{255, 255, 255, 180});

    float ry = py + 68.f;

    // Info denda
    DrawText("Denda keluar penjara:", (int)(px + 20), (int)ry, 12, {140, 140, 180, 255});
    std::string fineStr = "M" + std::to_string(jailDialog.jailFine);
    int fw = MeasureText(fineStr.c_str(), 15);
    DrawText(fineStr.c_str(), (int)(px + pw - 20 - fw), (int)(ry - 2), 15,
             jailDialog.canAffordFine ? Color{100, 220, 100, 255} : Color{220, 80, 80, 255});
    DrawLine((int)(px + 16), (int)(ry + 20), (int)(px + pw - 16), (int)(ry + 20), {60, 60, 90, 255});
    ry += 30.f;

    // Pesan hasil roll (jika sudah roll tapi gagal double)
    if (jailDialog.rolledThisTurn)
    {
        const char *msg = "Bukan double giliran ini tidak bergerak.";
        int mw = MeasureText(msg, 11);
        DrawText(msg, (int)(px + pw / 2 - mw / 2), (int)ry, 11, {220, 160, 60, 255});
        ry += 22.f;
    }

    Player *currP = nullptr;
    int freeCardIdx = -1;
    if (isRealMode())
    {
        currP = guiManager->getGameMaster()->getState().getCurrPlayer();
        freeCardIdx = findFreeJailCardIdx(currP);
    }
    bool hasFreeCard = (freeCardIdx >= 0);

    // ── Tombol-tombol ─────────────────────────────────────────────────────
    float btnH = 40.f;
    float btnW = pw - 32.f;
    float btnY1 = py + ph - 56.f - btnH * 2 - 16.f;
    float btnY2 = btnY1 + btnH + 8.f;
    float btnY3 = btnY2 + btnH + 8.f; 
    Vector2 mouse = GetMousePosition();

    // ── Tombol 1: BAYAR DENDA ────────────────────────────────────────────
    bool payDisabled = false; // Never disable
    Rectangle payBtn = {px + 16, btnY1, btnW, btnH};
    bool payHover = CheckCollisionPointRec(mouse, payBtn);
    Color payBg = payHover ? Color{60, 160, 80, 255}
                           : Color{40, 120, 60, 255};
    DrawRectangleRec(payBtn, payBg);
    DrawRectangleLinesEx(payBtn, 1.5f, Color{80, 200, 110, 255});
    std::string payLbl = "BAYAR DENDA  (M" + std::to_string(jailDialog.jailFine) + ")";
    int plw = MeasureText(payLbl.c_str(), 13);
    DrawText(payLbl.c_str(), (int)(payBtn.x + btnW / 2 - plw / 2), (int)(btnY1 + 15), 13, WHITE);

    // ── Tombol 2: COBA LEMPAR DADU / TUTUP (jika sudah roll) ─────────────
    bool rollDisabled = jailDialog.forcedPay || jailDialog.rolledThisTurn;
    Rectangle rollBtn = {px + 16, btnY2, btnW * 0.48f, btnH};
    bool rollHover = CheckCollisionPointRec(mouse, rollBtn) && !rollDisabled;
    Color rollBg = rollDisabled ? Color{40, 42, 54, 255}
                   : rollHover  ? Color{80, 120, 200, 255}
                                : Color{50, 80, 160, 255};
    DrawRectangleRec(rollBtn, rollBg);
    DrawRectangleLinesEx(rollBtn, 1.5f,
                         rollDisabled ? Color{60, 60, 80, 255} : Color{100, 150, 255, 255});
    const char *rollLbl = rollDisabled ? "LEMPAR DADU (N/A)" : "COBA LEMPAR DADU";
    int rlw = MeasureText(rollLbl, 11);
    DrawText(rollLbl, (int)(rollBtn.x + rollBtn.width / 2 - rlw / 2),
             (int)(btnY2 + 16), 11, rollDisabled ? Color{80, 80, 100, 255} : WHITE);

    // ── Tombol 3: TUTUP / LANJUT ──────────────────────────────────────────
    Rectangle closeBtn = {px + 16 + btnW * 0.52f, btnY2, btnW * 0.48f, btnH};
    bool closeHover = CheckCollisionPointRec(mouse, closeBtn);
    // Tombol tutup hanya aktif jika sudah roll (gagal) atau tidak ada aksi wajib
    bool closeActive = jailDialog.rolledThisTurn && !jailDialog.forcedPay;
    Color closeBg = !closeActive ? Color{40, 42, 54, 255}
                    : closeHover ? Color{160, 80, 50, 255}
                                 : Color{100, 50, 30, 255};
    DrawRectangleRec(closeBtn, closeBg);
    DrawRectangleLinesEx(closeBtn, 1.5f,
                         !closeActive ? Color{60, 60, 80, 255} : Color{200, 100, 80, 255});
    const char *closeLbl = "TUTUP";
    int clw = MeasureText(closeLbl, 12);
    DrawText(closeLbl, (int)(closeBtn.x + closeBtn.width / 2 - clw / 2),
             (int)(btnY2 + 16), 12, !closeActive ? Color{80, 80, 100, 255} : WHITE);

    if (hasFreeCard)
    {
        bool cardDisabled = jailDialog.rolledThisTurn; // sudah roll → tidak bisa
        Rectangle cardBtn = {px + 16, btnY3, btnW, btnH};
        bool cardHover = CheckCollisionPointRec(mouse, cardBtn) && !cardDisabled;

        Color cardBg = cardDisabled ? Color{40, 42, 54, 255}
                       : cardHover  ? Color{160, 120, 200, 255}
                                    : Color{100, 60, 160, 255};
        DrawRectangleRec(cardBtn, cardBg);
        DrawRectangleLinesEx(cardBtn, 1.5f,
                             cardDisabled ? Color{60, 60, 80, 255} : Color{200, 150, 255, 255});

        // Label: tampilkan nama kartu
        const auto &hand = currP->getHand();
        std::string cardName = "Kartu Bebas Penjara";
        if (freeCardIdx >= 0 && freeCardIdx < (int)hand.size())
            cardName = hand[freeCardIdx]->getType();
        std::string cardLbl = "GUNAKAN: " + cardName;
        int cwl = MeasureText(cardLbl.c_str(), 12);
        DrawText(cardLbl.c_str(), (int)(cardBtn.x + btnW / 2 - cwl / 2),
                 (int)(btnY3 + 13), 12,
                 cardDisabled ? Color{80, 80, 100, 255} : Color{230, 200, 255, 255});

        if (cardDisabled)
        {
            const char *hint = "(tidak bisa digunakan setelah lempar dadu)";
            int hw = MeasureText(hint, 9);
            DrawText(hint, (int)(cardBtn.x + btnW / 2 - hw / 2),
                     (int)(btnY3 + btnH + 4), 9, {120, 100, 140, 255});
        }

        // Handle klik kartu
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && cardHover && !cardDisabled && isRealMode())
        {
            GameMaster *gm = guiManager->getGameMaster();
            GameState &gs = gm->getState();
            Player* savedPlayer = currP;
            // Eksekusi kartu: releaseFromJail via FreeFromJailCard::execute()
            SkillCard *card = currP->getHand()[freeCardIdx];
            card->execute(*currP, gs);

            // Buang kartu dari tangan dan kembalikan ke deck
            SkillCard *removed = currP->discardSkillCard(freeCardIdx);
            if (removed && gs.getSkillDeck())
                gs.getSkillDeck()->discard(removed);

            gm->log(currP->getUsername(), "JAIL",
                    "Menggunakan kartu " + card->getType() + " untuk keluar penjara");

            // Player bebas → bisa langsung lempar dadu normal giliran ini
            gs.setHasRolled(false);
            jailDialog.visible = false;
            if (savedPlayer->getHandSize() > 3)
                gm->handleSkillCardOverflow(savedPlayer);
            else
                gs.setPhase(GamePhase::PLAYER_TURN);
            return;
        }
    }

    // ── Handle klik ───────────────────────────────────────────────────────
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {

        // BAYAR DENDA
        if (payHover)
        {
            if (guiManager && guiManager->getGameMaster())
            {
                GameMaster *gm = guiManager->getGameMaster();
                Player *p = gm->getState().getCurrPlayer();
                Bank *bank = gm->getState().getBank();
                Board *board = gm->getState().getBoard();

                if (p && bank && board)
                {
                    // Cari JailTile dan panggil payFine
                    for (int i = 0; i < board->getSize(); i++)
                    {
                        JailTile *jt = dynamic_cast<JailTile *>(board->getTile(i));
                        if (jt)
                        {
                            jt->payFine(*p, gm->getState());
                            if (gm->getState().getPendingDebt() <= 0) {
                                gm->log(p->getUsername(), "JAIL",
                                        "Membayar denda M" +
                                            std::to_string(jailDialog.jailFine) +
                                            " untuk keluar penjara");
                            }
                            break;
                        }
                    }
                    if (gm->getState().getPendingDebt() > 0)
                    {
                        gm->getState().setPhase(GamePhase::BANKRUPTCY);
                    }
                    else
                    {
                        if (p->getHandSize() > 3)
                            gm->handleSkillCardOverflow(p);
                        else
                            gm->getState().setPhase(GamePhase::PLAYER_TURN);
                    }
                }
            }
            jailDialog.visible = false;
        }

        // COBA LEMPAR DADU
        else if (rollHover && !rollDisabled)
        {
            if (guiManager && guiManager->getGameMaster())
            {
                GameMaster *gm = guiManager->getGameMaster();
                Player *p = gm->getState().getCurrPlayer();
                Dice *dice = gm->getState().getDice();

                if (p && dice)
                {
                    dice->rollRandom();
                    int v1 = dice->getDaduVal1();
                    int v2 = dice->getDaduVal2();

                    // Sync dice visual
                    diceState.val1 = v1;
                    diceState.val2 = v2;
                    diceState.isDouble = dice->isDouble();
                    diceState.animating = true;
                    diceState.animTimer = 0.f;

                    if (dice->isDouble())
                    {
                        // Berhasil double → keluar penjara, gerak
                        gm->releaseFromJail(p);
                        p->releaseFromJail();
                        gm->log(p->getUsername(), "JAIL",
                                "Keluar penjara dengan double " +
                                    std::to_string(v1) + "+" + std::to_string(v2));
                        gm->movePlayer(p, v1 + v2);
                        gm->getState().setHasRolled(true);
                        jailDialog.visible = false;
                        if (p->getHandSize() > 3)
                            gm->handleSkillCardOverflow(p);
                        else
                            gm->getState().setPhase(GamePhase::PLAYER_TURN);
                    }
                    else
                    {
                        // Update forced pay jika sekarang giliran ke-4
                        p->incrementJailTurns();
                        if (p->getJailTurns() >= 3)
                        {
                            jailDialog.forcedPay = true;
                            jailDialog.jailTurnsLeft = p->getJailTurns();
                            p->setJailTurns(0);
                        }
                        // Gagal double → tidak bergerak, tandai sudah roll
                      
                        gm->log(p->getUsername(), "JAIL",
                                "Gagal double " + std::to_string(v1) +
                                    "+" + std::to_string(v2) + " — tidak bergerak");
                        gm->getState().setHasRolled(true);
                        gm->getState().setHasExtraTurn(false);
                        jailDialog.rolledThisTurn = true;

                        
                    }
                }
            }
        }

        // TUTUP (setelah gagal roll)
        else if (closeHover && closeActive)
        {
            jailDialog.visible = false;
            if (guiManager && guiManager->getGameMaster())
            {
                GameMaster *gm = guiManager->getGameMaster();
                Player *p = gm->getState().getCurrPlayer();

                if (p && p->getHandSize() > 3)
                    gm->handleSkillCardOverflow(p); 
                else
                    gm->getState().setPhase(GamePhase::PLAYER_TURN);
            }
        }
    }
}