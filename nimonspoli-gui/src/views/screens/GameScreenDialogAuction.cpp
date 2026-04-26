#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/AuctionManager/AuctionManager.hpp"
#include "../../core/Bank/Bank.hpp"
#include <string>
#include <stdexcept>

// ─────────────────────────────────────────────
//  triggerAuctionDialog()
//  Dipanggil dari syncDiceResult() saat phase == AUCTION
// ─────────────────────────────────────────────
void GameScreen::triggerAuctionDialog()
{
    std::cout << "[DEBUG triggerAuction] called" << std::endl;
    if (!guiManager || !guiManager->getGameMaster()) {
        std::cout << "[DEBUG triggerAuction] no guiManager" << std::endl;
        return;
    }
    AuctionManager* am = guiManager->getGameMaster()->getState().getAuctionManager();
    std::cout << "[DEBUG triggerAuction] am=" << am 
              << " isOngoing=" << (am ? am->isOngoing() : -1) << std::endl;
    if (!am || !am->isOngoing()) return;
    Property* prop = am->getAuctionedProperty();
    if (!prop) return;

    auctionDialog.visible      = true;
    auctionDialog.propertyName = prop->getName();
    auctionDialog.propertyGroup = prop->getColorGroup();
    auctionDialog.bidInput     = "";
    auctionDialog.bidFocused   = false;

    // Sync state awal dari AuctionManager
    auctionDialog.currentBid     = am->getCurrentBid();
    auctionDialog.highestBidder  = am->getHighestBidder()
                                   ? am->getHighestBidder()->getUsername()
                                   : "-";
    auctionDialog.currParticipant = am->getCurrentParticipant()
                                    ? am->getCurrentParticipant()->getUsername()
                                    : "-";
    auctionDialog.isForcedBid    = am->isForcedBid();
}

// ─────────────────────────────────────────────
//  drawAuctionDialog()
//  Dipanggil setiap frame dari render() saat dialog visible
// ─────────────────────────────────────────────
void GameScreen::drawAuctionDialog()
{
    if (!auctionDialog.visible) return;

    // Sync state dari AuctionManager setiap frame
    if (guiManager && guiManager->getGameMaster()) {
        AuctionManager* am = guiManager->getGameMaster()->getState().getAuctionManager();
        if (am && am->isOngoing()) {
            auctionDialog.currentBid      = am->getCurrentBid();
            auctionDialog.highestBidder   = am->getHighestBidder()
                                            ? am->getHighestBidder()->getUsername()
                                            : "-";
            auctionDialog.currParticipant = am->getCurrentParticipant()
                                            ? am->getCurrentParticipant()->getUsername()
                                            : "-";
            auctionDialog.isForcedBid     = am->isForcedBid();
        }
    }

    // ── Cek apakah lelang sudah selesai ──────────────────────────────────
    if (guiManager && guiManager->getGameMaster()) {
        AuctionManager* am   = guiManager->getGameMaster()->getState().getAuctionManager();
        Bank*           bank = guiManager->getGameMaster()->getState().getBank();
        if (am && am->isAuctionOver()) {
            am->closeAuction(*bank);
            auctionDialog.visible = false;
            guiManager->getGameMaster()->getState().setPhase(GamePhase::PLAYER_TURN);
            return;
        }
    }

    // ── Overlay ───────────────────────────────────────────────────────────
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 170});

    float pw = 480.f, ph = 380.f;
    float px = SCREEN_W / 2.f - pw / 2.f;
    float py = SCREEN_H / 2.f - ph / 2.f;

    // Panel utama
    DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {25, 27, 38, 255});
    DrawRectangleLinesEx({px, py, pw, ph}, 2.f, {80, 80, 130, 255});

    // Header
    Color hdrCol = auctionDialog.propertyGroup.empty()
                   ? Color{50, 52, 70, 255}
                   : getGroupColor(auctionDialog.propertyGroup);
    DrawRectangle((int)px, (int)py, (int)pw, 52, hdrCol);
    std::string title = "LELANG: " + auctionDialog.propertyName;
    int tw = MeasureText(title.c_str(), 17);
    DrawText(title.c_str(), (int)(px + pw/2 - tw/2), (int)(py + 8), 17, WHITE);
    int gw = MeasureText(auctionDialog.propertyGroup.c_str(), 11);
    DrawText(auctionDialog.propertyGroup.c_str(),
             (int)(px + pw/2 - gw/2), (int)(py + 32), 11, {255,255,255,180});

    float ry = py + 66.f;

    // ── Bid tertinggi saat ini ────────────────────────────────────────────
    DrawText("Bid Tertinggi", (int)(px + 20), (int)ry, 12, {140,140,180,255});
    std::string bidStr = auctionDialog.currentBid > 0
                         ? "M" + std::to_string(auctionDialog.currentBid)
                           + "  (" + auctionDialog.highestBidder + ")"
                         : "Belum ada";
    int bw = MeasureText(bidStr.c_str(), 14);
    DrawText(bidStr.c_str(), (int)(px + pw - 20 - bw), (int)(ry - 1), 14,
             auctionDialog.currentBid > 0 ? Color{100,220,100,255} : Color{160,160,180,255});
    DrawLine((int)(px+16), (int)(ry+18), (int)(px+pw-16), (int)(ry+18), {60,60,90,255});
    ry += 28.f;

    // ── Giliran siapa ─────────────────────────────────────────────────────
    DrawText("Giliran", (int)(px + 20), (int)ry, 12, {140,140,180,255});
    int cw = MeasureText(auctionDialog.currParticipant.c_str(), 15);
    DrawText(auctionDialog.currParticipant.c_str(),
             (int)(px + pw - 20 - cw), (int)(ry - 2), 15, WHITE);
    DrawLine((int)(px+16), (int)(ry+20), (int)(px+pw-16), (int)(ry+20), {60,60,90,255});
    ry += 30.f;

    // ── Forced bid warning ────────────────────────────────────────────────
    if (auctionDialog.isForcedBid) {
        const char* warn = "Semua pemain sudah PASS — wajib melakukan BID!";
        int ww = MeasureText(warn, 10);
        DrawText(warn, (int)(px + pw/2 - ww/2), (int)ry, 10, {220, 120, 60, 255});
        ry += 20.f;
    }

    // ── Input field BID ───────────────────────────────────────────────────
    DrawText("Jumlah Bid:", (int)(px + 20), (int)ry, 12, {140,140,180,255});
    ry += 20.f;

    Rectangle inputBox = {px + 20, ry, pw - 40, 36};
    Color inputBorder  = auctionDialog.bidFocused
                         ? Color{100, 160, 255, 255}
                         : Color{80, 80, 120, 255};
    DrawRectangleRec(inputBox, {35, 37, 55, 255});
    DrawRectangleLinesEx(inputBox, 1.5f, inputBorder);

    std::string displayInput = auctionDialog.bidInput.empty()
                               ? "Masukkan jumlah..."
                               : "M" + auctionDialog.bidInput;
    Color inputTextCol = auctionDialog.bidInput.empty()
                         ? Color{90, 90, 120, 255}
                         : Color{255, 255, 255, 255};
    DrawText(displayInput.c_str(), (int)(inputBox.x + 10), (int)(inputBox.y + 10),
             13, inputTextCol);

    // Cursor berkedip
    if (auctionDialog.bidFocused && ((int)(GetTime() * 2) % 2 == 0) &&
        !auctionDialog.bidInput.empty()) {
        int cursorX = (int)(inputBox.x + 10 +
                      MeasureText(("M" + auctionDialog.bidInput).c_str(), 13));
        DrawLine(cursorX, (int)(inputBox.y + 6),
                 cursorX, (int)(inputBox.y + 30), WHITE);
    }

    // Klik input field → aktifkan focus
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(GetMousePosition(), inputBox)) {
        auctionDialog.bidFocused = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        auctionDialog.bidFocused = false;
    }

    // Handle keyboard input (hanya angka)
    if (auctionDialog.bidFocused) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && auctionDialog.bidInput.size() < 9)
                auctionDialog.bidInput += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !auctionDialog.bidInput.empty())
            auctionDialog.bidInput.pop_back();
    }

    ry += 46.f;

    // ── Tombol BID dan PASS ────────────────────────────────────────────────
    float btnW    = pw / 2.f - 24.f;
    float btnY    = py + ph - 56.f;
    Vector2 mouse = GetMousePosition();

    // Tombol BID
    int bidAmount = 0;
    bool bidValid = false;
    if (!auctionDialog.bidInput.empty()) {
        try {
            bidAmount = std::stoi(auctionDialog.bidInput);
            // Valid jika lebih tinggi dari bid saat ini
            bidValid  = bidAmount > auctionDialog.currentBid;

            // Cek apakah pemain mampu
            if (guiManager && guiManager->getGameMaster()) {
                AuctionManager* am = guiManager->getGameMaster()
                                     ->getState().getAuctionManager();
                if (am && am->getCurrentParticipant())
                    bidValid = bidValid &&
                               am->getCurrentParticipant()->canAfford(bidAmount);
            }
        } catch (...) {
            bidValid = false;
        }
    }

    Rectangle bidBtn  = {px + 16, btnY, btnW, 40};
    bool bidHover     = CheckCollisionPointRec(mouse, bidBtn) && bidValid;
    Color bidBg       = !bidValid   ? Color{40, 42, 54, 255}
                      : bidHover    ? Color{60, 180, 90, 255}
                                    : Color{40, 140, 70, 255};
    DrawRectangleRec(bidBtn, bidBg);
    DrawRectangleLinesEx(bidBtn, 1.5f,
                         !bidValid ? Color{60,60,80,255} : Color{80,200,110,255});
    const char* bidLbl = bidValid ? "BID" : (auctionDialog.bidInput.empty() ? "BID" : "BID (TIDAK VALID)");
    int blw = MeasureText(bidLbl, 13);
    DrawText(bidLbl, (int)(bidBtn.x + btnW/2 - blw/2), (int)(btnY + 13), 13,
             !bidValid ? Color{80,80,100,255} : WHITE);

    // Tombol PASS
    bool passDisabled = auctionDialog.isForcedBid;
    Rectangle passBtn = {px + pw/2 + 8, btnY, btnW, 40};
    bool passHover    = CheckCollisionPointRec(mouse, passBtn) && !passDisabled;
    Color passBg      = passDisabled ? Color{40, 42, 54, 255}
                      : passHover   ? Color{180, 80, 50, 255}
                                    : Color{120, 50, 30, 255};
    DrawRectangleRec(passBtn, passBg);
    DrawRectangleLinesEx(passBtn, 1.5f,
                         passDisabled ? Color{60,60,80,255} : Color{200,100,80,255});
    const char* passLbl = passDisabled ? "PASS (WAJIB BID)" : "PASS";
    int plw = MeasureText(passLbl, 12);
    DrawText(passLbl, (int)(passBtn.x + btnW/2 - plw/2), (int)(btnY + 14), 12,
             passDisabled ? Color{80,80,100,255} : WHITE);

    // ── Handle klik tombol ────────────────────────────────────────────────
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (bidHover && bidValid) {
            if (guiManager && guiManager->getGameMaster()) {
                AuctionManager* am = guiManager->getGameMaster()
                                     ->getState().getAuctionManager();
                Player* curr = am ? am->getCurrentParticipant() : nullptr;
                if (am && curr) {
                    am->placeBid(curr, bidAmount);
                    guiManager->getGameMaster()->log(
                        curr->getUsername(), "LELANG",
                        "BID M" + std::to_string(bidAmount) +
                        " untuk " + auctionDialog.propertyName);
                }
            }
            auctionDialog.bidInput   = "";
            auctionDialog.bidFocused = false;

        } else if (passHover && !passDisabled) {
            if (guiManager && guiManager->getGameMaster()) {
                AuctionManager* am = guiManager->getGameMaster()
                                     ->getState().getAuctionManager();
                Player* curr = am ? am->getCurrentParticipant() : nullptr;
                if (am && curr) {
                    am->passBid();
                    guiManager->getGameMaster()->log(
                        curr->getUsername(), "LELANG",
                        "PASS untuk " + auctionDialog.propertyName);
                }
            }
            auctionDialog.bidInput   = "";
            auctionDialog.bidFocused = false;
        }
    }
}