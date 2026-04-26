// GameScreenDialogBankruptcy.cpp
// Dialog likuidasi (BANKRUPTCY phase) dan notifikasi kebangkrutan.
//
// Alur:
//   1. triggerBankruptcyDialog() dipanggil dari syncDiceResult() saat phase == BANKRUPTCY.
//   2. Dialog menampilkan daftar properti milik pemain aktif beserta opsi JUAL / GADAI.
//   3. Setelah balance >= pendingDebt, tombol SELESAI aktif → proses pembayaran.
//   4. Jika tidak ada lagi properti dan balance masih kurang → bangkrut langsung.

#include "GameScreen.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Board/Board.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Player/Player.hpp"

// ─────────────────────────────────────────────────────────────────────────────
//  triggerBankruptcyDialog()
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::triggerBankruptcyDialog()
{
    if (!isRealMode()) return;

    GameMaster*       gm    = guiManager->getGameMaster();
    GameState&        gs    = gm->getState();
    // Gunakan pendingDebtor jika ada; fallback ke currPlayer (kasus sewa/pajak)
    Player*           cur   = gs.getPendingDebtor() ? gs.getPendingDebtor() : gs.getCurrPlayer();
    Board*            board = gs.getBoard();

    if (!cur || !board) return;

    int  debt     = gs.getPendingDebt();
    Player* cred  = gs.getPendingCreditor();

    if (debt <= 0) debt = 0;

    bankruptcyDialog.visible      = true;
    bankruptcyDialog.notifVisible = false;
    bankruptcyDialog.scrollY      = 0.f;
    bankruptcyDialog.pendingDebt  = debt;
    bankruptcyDialog.creditorName = cred ? cred->getUsername() : "Bank";
    bankruptcyDialog.entries.clear();
    bankruptcyDialog.hoveredSellIdx     = -1;
    bankruptcyDialog.hoveredMortgageIdx = -1;
    bankruptcyDialog.bankruptName       = cur->getUsername();

    // Bangun daftar properti yang bisa dilikuidasi
    for (int i = 0; i < board->getSize(); ++i)
    {
        auto* pt = dynamic_cast<PropertyTile*>(board->getTile(i));
        if (!pt || !pt->getProperty()) continue;
        Property* p = pt->getProperty();
        if (p->getOwnerId() != cur->getUsername()) continue;

        BankruptcyDialogState::Entry entry;
        entry.prop     = p;
        entry.tileIdx  = i;

        // Bisa dijual ke bank jika tidak sedang digadaikan
        entry.canSell = (p->getStatus() != PropertyStatus::MORTGAGED);
        if (entry.canSell)
        {
            auto* sp = dynamic_cast<StreetProperty*>(p);
            entry.sellValue = sp ? sp->calculateSellPrice() : p->getPurchasePrice();
        }
        else
        {
            entry.sellValue = 0;
        }

        // Bisa digadaikan jika OWNED dan tidak punya bangunan
        entry.canMortgage = (p->getStatus() == PropertyStatus::OWNED);
        if (entry.canMortgage)
        {
            auto* sp = dynamic_cast<StreetProperty*>(p);
            if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel()))
                entry.canMortgage = false; // harus jual bangunan dulu
        }
        entry.mortgageValue = entry.canMortgage ? p->getMortageValue() : 0;

        bankruptcyDialog.entries.push_back(entry);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  drawBankruptcyDialog()
// ─────────────────────────────────────────────────────────────────────────────
void GameScreen::drawBankruptcyDialog()
{
    if (!bankruptcyDialog.visible && !bankruptcyDialog.notifVisible) return;

    // ── Notifikasi: pemain bangkrut ───────────────────────────────────────
    if (bankruptcyDialog.notifVisible)
    {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 200});

        float pw = 500.f, ph = 260.f;
        float px = SCREEN_W / 2.f - pw / 2.f;
        float py = SCREEN_H / 2.f - ph / 2.f;

        // Panel
        DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {28, 12, 12, 255});
        DrawRectangleLinesEx({px, py, pw, ph}, 2.5f, {220, 50, 50, 255});

        // Header merah
        DrawRectangle((int)px, (int)py, (int)pw, 54, {180, 30, 30, 255});
        const char* hdr = "BANGKRUT!";
        int hw = MeasureText(hdr, 24);
        DrawText(hdr, (int)(px + pw / 2 - hw / 2), (int)(py + 14), 24, WHITE);

        // Nama pemain
        std::string msg1 = bankruptcyDialog.bankruptName + " tidak mampu membayar kewajiban.";
        int mw1 = MeasureText(msg1.c_str(), 13);
        DrawText(msg1.c_str(), (int)(px + pw / 2 - mw1 / 2), (int)(py + 72), 13, {230, 230, 230, 255});

        std::string msg2 = "Seluruh aset diserahkan ke " + bankruptcyDialog.creditorName + ".";
        int mw2 = MeasureText(msg2.c_str(), 13);
        DrawText(msg2.c_str(), (int)(px + pw / 2 - mw2 / 2), (int)(py + 94), 13, {200, 180, 180, 255});

        std::string msg3 = bankruptcyDialog.bankruptName + " keluar dari permainan.";
        int mw3 = MeasureText(msg3.c_str(), 12);
        DrawText(msg3.c_str(), (int)(px + pw / 2 - mw3 / 2), (int)(py + 118), 12, {170, 140, 140, 255});

        // Tombol OK
        Rectangle okBtn = {px + pw / 2 - 70, py + ph - 56, 140, 38};
        bool okHov = CheckCollisionPointRec(GetMousePosition(), okBtn);
        DrawRectangleRec(okBtn, okHov ? Color{200, 60, 60, 255} : Color{140, 30, 30, 255});
        DrawRectangleLinesEx(okBtn, 1.5f, {220, 80, 80, 255});
        const char* ok = "OK";
        int ow = MeasureText(ok, 14);
        DrawText(ok, (int)(okBtn.x + 70 - ow / 2), (int)(okBtn.y + 11), 14, WHITE);

        if (okHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            bankruptcyDialog.notifVisible = false;
            // Jika masih ada antrian pembayaran kartu (Election/Birthday),
            // lanjutkan pembayaran berikutnya.
            GameMaster* gm2 = guiManager->getGameMaster();
            if (gm2 && gm2->getState().hasPendingPayment())
                gm2->processNextCardPayment();
        }
        return;
    }

    // ── Dialog likuidasi ──────────────────────────────────────────────────
    if (!bankruptcyDialog.visible) return;
    if (!isRealMode()) return;

    GameMaster* gm  = guiManager->getGameMaster();
    GameState&  gs  = gm->getState();
    // Gunakan pendingDebtor agar cocok dengan kartu multi-pemain
    Player* cur = gs.getPendingDebtor() ? gs.getPendingDebtor() : gs.getCurrPlayer();
    if (!cur) return;

    // Re-sync entries (saldo dan daftar properti bisa berubah setelah aksi)
    // Lakukan re-trigger ringan: perbarui nilai (tapi jangan hapus list)
    for (auto& e : bankruptcyDialog.entries)
    {
        if (!e.prop) continue;
        e.canSell = (e.prop->getStatus() != PropertyStatus::MORTGAGED);
        if (e.canSell)
        {
            auto* sp = dynamic_cast<StreetProperty*>(e.prop);
            e.sellValue = sp ? sp->calculateSellPrice() : e.prop->getPurchasePrice();
        }
        else e.sellValue = 0;

        e.canMortgage = (e.prop->getStatus() == PropertyStatus::OWNED);
        if (e.canMortgage)
        {
            auto* sp = dynamic_cast<StreetProperty*>(e.prop);
            if (sp && (sp->getBuildingCount() > 0 || sp->gethasHotel()))
                e.canMortgage = false;
        }
        e.mortgageValue = e.canMortgage ? e.prop->getMortageValue() : 0;
    }

    int  balance    = cur->getBalance();
    int  debt       = bankruptcyDialog.pendingDebt;
    int  shortfall  = debt - balance;
    bool canFinish  = (balance >= debt);

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 190});

    float pw = 640.f, ph = 520.f;
    float px = SCREEN_W / 2.f - pw / 2.f;
    float py = SCREEN_H / 2.f - ph / 2.f;

    // Panel utama
    DrawRectangle((int)px, (int)py, (int)pw, (int)ph, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, pw, ph}, 2.f, {180, 60, 60, 255});

    // Header
    DrawRectangle((int)px, (int)py, (int)pw, 52, {100, 30, 30, 255});
    const char* title = "LIKUIDASI ASET";
    int tw = MeasureText(title, 18);
    DrawText(title, (int)(px + pw / 2 - tw / 2), (int)(py + 16), 18, WHITE);

    // Info hutang
    float ry = py + 62.f;
    std::string infoDebt = "Hutang: M" + std::to_string(debt) +
                           "  ke  " + bankruptcyDialog.creditorName;
    DrawText(infoDebt.c_str(), (int)(px + 16), (int)ry, 13, {220, 140, 140, 255});

    std::string infoCash = "Saldo: M" + std::to_string(balance);
    Color cashCol = canFinish ? Color{100, 220, 100, 255} : Color{220, 100, 100, 255};
    int cw = MeasureText(infoCash.c_str(), 13);
    DrawText(infoCash.c_str(), (int)(px + pw - 16 - cw), (int)ry, 13, cashCol);

    ry += 22.f;
    if (!canFinish)
    {
        std::string infoShort = "Kurang: M" + std::to_string(shortfall) +
                                "  — Pilih properti untuk dilikuidasi.";
        int sw = MeasureText(infoShort.c_str(), 11);
        DrawText(infoShort.c_str(), (int)(px + pw / 2 - sw / 2), (int)ry, 11,
                 {220, 160, 60, 255});
    }
    else
    {
        const char* ok = "Saldo sudah cukup. Klik SELESAI untuk melanjutkan.";
        int sw = MeasureText(ok, 11);
        DrawText(ok, (int)(px + pw / 2 - sw / 2), (int)ry, 11, {100, 220, 100, 255});
    }

    DrawLine((int)(px + 12), (int)(ry + 20), (int)(px + pw - 12), (int)(ry + 20),
             {70, 40, 40, 255});
    ry += 28.f;

    // Scroll area properti
    float listH    = ph - (ry - py) - 58.f;
    float rowH     = 44.f;
    Vector2 mouse  = GetMousePosition();

    // Scissor (clipping) — ganti dengan manual bound check agar kompatibel
    for (size_t i = 0; i < bankruptcyDialog.entries.size(); ++i)
    {
        auto& e = bankruptcyDialog.entries[i];
        if (!e.prop) continue;
        if (e.prop->getOwnerId() != cur->getUsername()) continue; // sudah terjual/digadai?

        float rowY = ry + (float)i * rowH - bankruptcyDialog.scrollY;
        if (rowY + rowH < ry || rowY > ry + listH) continue; // di luar viewport

        Rectangle row = {px + 10, rowY, pw - 20, rowH - 4};
        DrawRectangleRec(row, {30, 32, 48, 255});
        DrawRectangleLinesEx(row, 1.f, {60, 40, 40, 255});

        // Nama properti
        DrawText(e.prop->getName().c_str(), (int)(row.x + 8), (int)(row.y + 6), 12, WHITE);
        // Status
        const char* statStr = (e.prop->getStatus() == PropertyStatus::MORTGAGED)
                              ? "[GADAI]" : "[OWNED]";
        Color statCol = (e.prop->getStatus() == PropertyStatus::MORTGAGED)
                        ? Color{200, 160, 60, 255} : Color{100, 200, 100, 255};
        DrawText(statStr, (int)(row.x + 8), (int)(row.y + 24), 10, statCol);

        // Tombol JUAL
        float btnW = 110.f;
        Rectangle sellBtn = {row.x + row.width - btnW * 2 - 14, row.y + 6, btnW, 32};
        bool sellHov      = CheckCollisionPointRec(mouse, sellBtn) && e.canSell;
        Color sellBg      = !e.canSell   ? Color{40, 40, 50, 255}
                          : sellHov      ? Color{60, 180, 80, 255}
                                         : Color{40, 120, 60, 255};
        DrawRectangleRec(sellBtn, sellBg);
        DrawRectangleLinesEx(sellBtn, 1.f,
                             !e.canSell ? Color{60, 60, 80, 255} : Color{80, 200, 110, 255});
        std::string sellLbl = e.canSell
                              ? ("JUAL M" + std::to_string(e.sellValue))
                              : "JUAL (-)";
        int slw = MeasureText(sellLbl.c_str(), 10);
        DrawText(sellLbl.c_str(),
                 (int)(sellBtn.x + btnW / 2 - slw / 2),
                 (int)(sellBtn.y + 10), 10,
                 !e.canSell ? Color{80, 80, 100, 255} : WHITE);

        // Tombol GADAI
        Rectangle gadaiBtn = {row.x + row.width - btnW - 6, row.y + 6, btnW, 32};
        bool gadaiHov      = CheckCollisionPointRec(mouse, gadaiBtn) && e.canMortgage;
        Color gadaiBg      = !e.canMortgage ? Color{40, 40, 50, 255}
                           : gadaiHov       ? Color{200, 140, 40, 255}
                                            : Color{140, 90, 20, 255};
        DrawRectangleRec(gadaiBtn, gadaiBg);
        DrawRectangleLinesEx(gadaiBtn, 1.f,
                             !e.canMortgage ? Color{60, 60, 80, 255} : Color{220, 170, 60, 255});
        std::string gadaiLbl = e.canMortgage
                               ? ("GADAI M" + std::to_string(e.mortgageValue))
                               : "GADAI (-)";
        int glw = MeasureText(gadaiLbl.c_str(), 10);
        DrawText(gadaiLbl.c_str(),
                 (int)(gadaiBtn.x + btnW / 2 - glw / 2),
                 (int)(gadaiBtn.y + 10), 10,
                 !e.canMortgage ? Color{80, 80, 100, 255} : WHITE);

        // Klik JUAL
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && sellHov && e.canSell)
        {
            gm->sellPropertyToBank(cur, e.prop);
            // Re-trigger untuk refresh daftar
            triggerBankruptcyDialog();
            return;
        }
        // Klik GADAI
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && gadaiHov && e.canMortgage)
        {
            gm->mortgageProperty(cur, e.prop);
            triggerBankruptcyDialog();
            return;
        }
    }

    // Scroll wheel
    if (CheckCollisionPointRec(mouse, {px, ry, pw, listH}))
    {
        float wheel = GetMouseWheelMove();
        float maxScroll = std::max(0.f,
            (float)bankruptcyDialog.entries.size() * rowH - listH);
        bankruptcyDialog.scrollY = std::max(0.f,
            std::min(bankruptcyDialog.scrollY - wheel * 30.f, maxScroll));
    }

    // ── Tombol bawah ──────────────────────────────────────────────────────
    float btnY = py + ph - 48.f;
    DrawLine((int)(px + 12), (int)btnY - 4, (int)(px + pw - 12), (int)btnY - 4,
             {70, 40, 40, 255});

    // SELESAI (bayar hutang)
    Rectangle finishBtn = {px + pw / 2 - 70, btnY + 4, 140, 34};
    bool finishHov      = CheckCollisionPointRec(mouse, finishBtn) && canFinish;
    Color finBg         = !canFinish ? Color{40, 42, 54, 255}
                        : finishHov  ? Color{60, 180, 80, 255}
                                     : Color{40, 130, 60, 255};
    DrawRectangleRec(finishBtn, finBg);
    DrawRectangleLinesEx(finishBtn, 1.5f,
                         !canFinish ? Color{60, 60, 80, 255} : Color{80, 200, 110, 255});
    const char* finLbl = canFinish ? "SELESAI (BAYAR)" : "SELESAI (belum cukup)";
    int flw = MeasureText(finLbl, 11);
    DrawText(finLbl, (int)(finishBtn.x + 70 - flw / 2), (int)(finishBtn.y + 11), 11,
             !canFinish ? Color{80, 80, 100, 255} : WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && finishHov && canFinish)
    {
        // Bayar hutang
        Player* creditor = gs.getPendingCreditor();
        int     debt2    = gs.getPendingDebt();
        if (creditor) { *cur -= debt2; *creditor += debt2; }
        else          { *cur -= debt2; }

        gm->log(cur->getUsername(), "BAYAR_HUTANG",
                "Membayar M" + std::to_string(debt2) +
                " ke " + bankruptcyDialog.creditorName + " setelah likuidasi");

        gs.setPendingDebt(0);
        gs.setPendingDebtor(nullptr);
        gs.setPendingCreditor(nullptr);
        bankruptcyDialog.visible = false;

        // Lanjutkan pembayaran berikutnya jika masih ada antrian kartu
        if (gs.hasPendingPayment())
            gm->processNextCardPayment();
        else
            gs.setPhase(GamePhase::PLAYER_TURN);
    }

    // Cek apakah semua properti sudah habis dan saldo masih kurang
    bool noMoreProps = true;
    for (auto& e : bankruptcyDialog.entries)
    {
        if (e.prop && e.prop->getOwnerId() == cur->getUsername())
        { noMoreProps = false; break; }
    }

    if (noMoreProps && !canFinish)
    {
        // Tidak bisa bayar → bangkrut
        bankruptcyDialog.visible      = false;
        bankruptcyDialog.notifVisible = true;
        bankruptcyDialog.bankruptName = cur->getUsername();

        Player* creditor = gs.getPendingCreditor();
        if (creditor)
            gm->handleBankruptcy(cur, creditor);
        else
            gm->handleBankruptcy(cur, gs.getBank());
    }
}
