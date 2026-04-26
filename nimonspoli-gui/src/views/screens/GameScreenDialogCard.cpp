#include "GameScreen.hpp"
#include "GameScreenTiles.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Player/Player.hpp"
#include <string>
// ─────────────────────────────────────────────────────────────────────────────
//  CardDialog
// ─────────────────────────────────────────────────────────────────────────────

static std::string getCardTextureKey(const std::string &deckLabel,
                                     const std::string &description)
{
    // DNU (Dana Umum)
    if (deckLabel == "Dana Umum" || deckLabel == "DNU")
    {
        if (description.find("ulang tahun") != std::string::npos ||
            description.find("Birthday") != std::string::npos)
            return "DNU-Birthday";
        if (description.find("dokter") != std::string::npos ||
            description.find("Doctor") != std::string::npos)
            return "DNU-DoctorFee";
        if (description.find("nyaleg") != std::string::npos ||
            description.find("Election") != std::string::npos ||
            description.find("run for") != std::string::npos)
            return "DNU-RunForOffice";
    }
    // KSP (Kesempatan / ChanceCard)
    if (deckLabel == "Kesempatan" || deckLabel == "KSP")
    {
        if (description.find("Penjara") != std::string::npos ||
            description.find("Jail") != std::string::npos)
            return "KSP-GoToJail";
        if (description.find("Mundur") != std::string::npos ||
            description.find("mundur") != std::string::npos)
            return "KSP-MoveBack";
        if (description.find("stasiun") != std::string::npos ||
            description.find("Station") != std::string::npos)
            return "KSP-NearStation";
    }
    return "";
}

static void DrawWrappedText(
    const std::string &text,
    float x,
    float y,
    float maxWidth,
    int fontSize,
    float lineSpacing,
    Color color)
{
    std::string line;
    std::string word;

    float curY = y;

    for (size_t i = 0; i <= text.size(); i++)
    {
        char c = (i < text.size()) ? text[i] : ' ';

        if (c == ' ' || c == '\n' || i == text.size())
        {
            if (!word.empty())
            {
                std::string testLine = line.empty() ? word : line + " " + word;

                if (MeasureText(testLine.c_str(), fontSize) > maxWidth && !line.empty())
                {
                    DrawText(line.c_str(), (int)x, (int)curY, fontSize, color);
                    curY += fontSize + lineSpacing;
                    line = word;
                }
                else
                {
                    line = testLine;
                }

                word.clear();
            }

            if (c == '\n')
            {
                DrawText(line.c_str(), (int)x, (int)curY, fontSize, color);
                curY += fontSize + lineSpacing;
                line.clear();
            }
        }
        else
        {
            word += c;
        }
    }

    if (!line.empty())
        DrawText(line.c_str(), (int)x, (int)curY, fontSize, color);
}

void GameScreen::triggerCardDialog()
{
    if (!isRealMode())
        return;
    const GameState &gs = guiManager->getGameMaster()->getState();
    cardDialog.deckLabel = gs.getPendingCardDeck();
    cardDialog.description = gs.getPendingCardDesc();
    cardDialog.visible = true;
}

void GameScreen::drawCardDialog()
{
    if (!cardDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 520.f, PH = 300.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    bool isKSP = (cardDialog.deckLabel == "Kesempatan");
    Color hdrCol = isKSP ? Color{180, 120, 0, 255} : Color{60, 60, 200, 255};
    Color bordCol = isKSP ? Color{240, 180, 40, 255} : Color{100, 120, 240, 255};

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, bordCol);
    DrawRectangle((int)px, (int)py, (int)PW, 48, hdrCol);

    // Header label di tengah
    std::string dl = cardDialog.deckLabel;
    int dlw = MeasureText(dl.c_str(), 17);
    DrawText(dl.c_str(), (int)(px + PW / 2 - dlw / 2), (int)(py + 15), 17, WHITE);

    // ── Cari texture ──────────────────────────────────────────────
    std::string texKey = getCardTextureKey(cardDialog.deckLabel,
                                           cardDialog.description);

    constexpr float IMG_W = 130.f;
    constexpr float IMG_H = PH - 48.f - 70.f; // sisakan ruang tombol
    float imgX = px + 16.f;
    float imgY = py + 58.f;

    if (!texKey.empty() && cardTextures.count(texKey))
    {
        Texture2D &tex = cardTextures[texKey];
        // Scale agar muat, jaga aspek rasio
        float scaleX = IMG_W / tex.width;
        float scaleY = IMG_H / tex.height;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float dw = tex.width * scale;
        float dh = tex.height * scale;
        float dx = imgX + (IMG_W - dw) / 2.f;
        float dy = imgY + (IMG_H - dh) / 2.f;
        DrawTextureEx(tex, {dx, dy}, 0.f, scale, WHITE);
    }
    else
    {
        // Fallback: kotak placeholder dengan ikon
        DrawRectangle((int)imgX, (int)imgY, (int)IMG_W, (int)IMG_H, {35, 38, 55, 255});
        DrawRectangleLinesEx({imgX, imgY, IMG_W, IMG_H}, 1.f, bordCol);
        const char *icon = isKSP ? "?" : "$";
        int iw = MeasureText(icon, 28);
        DrawText(icon, (int)(imgX + IMG_W / 2 - iw / 2),
                 (int)(imgY + IMG_H / 2 - 16), 28, hdrCol);
    }

    // ── Teks deskripsi di sebelah kanan gambar ────────────────────
    float textX = px + 16.f + IMG_W + 12.f;
    float textW = PW - IMG_W - 16.f - 12.f - 16.f;
    float textY = py + 62.f;

    DrawWrappedText(cardDialog.description,
                    textX, textY, textW,
                    13, 6, {220, 220, 240, 255});

    // ── Tombol OK ─────────────────────────────────────────────────
    DrawLine((int)(px + 16), (int)(py + PH - 62),
             (int)(px + PW - 16), (int)(py + PH - 62), {60, 60, 90, 255});
    float btnW = 120.f;
    Rectangle okBtn = {px + PW / 2 - btnW / 2, py + PH - 50, btnW, 36};
    bool okHov = CheckCollisionPointRec(GetMousePosition(), okBtn);
    DrawRectangleRec(okBtn, okHov ? Color{80, 160, 100, 255} : Color{50, 110, 70, 255});
    DrawRectangleLinesEx(okBtn, 1.5f, {100, 200, 130, 255});
    int okw = MeasureText("OK", 14);
    DrawText("OK", (int)(okBtn.x + btnW / 2 - okw / 2), (int)(okBtn.y + 10), 14, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && okHov)
    {
        if (isRealMode())
        {
            GameMaster* gm = guiManager->getGameMaster();
            GameState& gs = gm->getState();
            if (gs.getPendingDebt() > 0 || gs.hasPendingPayment())
            {
                gs.setPhase(GamePhase::BANKRUPTCY);
            }
            else
            {
                gs.setPhase(GamePhase::PLAYER_TURN);
            }
        }
        cardDialog.visible = false;
    }
}
// ─────────────────────────────────────────────────────────────────────────────
//  JailDialog
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
//  PropertiPopup — daftar properti milik pemain aktif
// ─────────────────────────────────────────────────────────────────────────────

void GameScreen::drawPropertiPopup()
{
    if (!propertiPopup.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 520.f, PH = 500.f;
    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, {80, 80, 130, 255});
    DrawRectangle((int)px, (int)py, (int)PW, 44, {32, 34, 52, 255});
    DrawRectangleLinesEx({px, py, PW, 44}, 1, {80, 80, 130, 255});
    DrawText("DAFTAR PROPERTI", (int)(px + 16), (int)(py + 14), 14, {180, 180, 230, 255});

    // Tombol X
    Rectangle xBtn = {px + PW - 36, py + 8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);
    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
        propertiPopup.visible = false;

    // Daftar properti milik pemain aktif
    float listX = px + 16, listY = py + 52;
    float listW = PW - 32, listH = PH - 60;
    float rowH = 54.f;

    std::vector<int> owned;
    for (int i = 0; i < (int)gameState.properties.size(); i++)
        if (gameState.properties[i].owner == gameState.activePlayerIdx)
            owned.push_back(i);
    float totalH = owned.size() * rowH;

    Rectangle listArea = {listX, listY, listW, listH};
    if (CheckCollisionPointRec(GetMousePosition(), listArea))
    {
        float wheel = GetMouseWheelMove();
        propertiPopup.scrollY -= wheel * 40.f;
        propertiPopup.scrollY = std::max(0.f,
                                         std::min(propertiPopup.scrollY, std::max(0.f, totalH - listH)));
    }

    BeginScissorMode((int)listX, (int)listY, (int)listW, (int)listH);
    Color pCol = playerColors[gameState.activePlayerIdx];
    for (int ii = 0; ii < (int)owned.size(); ii++)
    {
        int i = owned[ii];
        auto &prop = gameState.properties[i];
        float ry = listY + ii * rowH - propertiPopup.scrollY;
        if (ry + rowH < listY || ry > listY + listH)
            continue;

        Color rowBg = {30, 32, 48, 255};
        DrawRectangle((int)listX, (int)ry, (int)listW, (int)(rowH - 4), rowBg);
        DrawRectangleLinesEx({listX, ry, listW, rowH - 4}, 1.f, {60, 60, 90, 255});

        // Color swatch
        Color grpCol = prop.colorGroup.empty() ? GRAY : getGroupColor(prop.colorGroup);
        DrawRectangle((int)listX, (int)ry, 8, (int)(rowH - 4), grpCol);

        std::string pname = prop.name.empty() ? prop.code : prop.name;
        DrawText(pname.c_str(), (int)(listX + 16), (int)(ry + 8), 13, WHITE);

        // Buildings
        std::string bldStr = prop.buildings == 5 ? "Hotel" : prop.buildings > 0 ? std::to_string(prop.buildings) + " rumah"
                                                                                : "-";
        DrawText(bldStr.c_str(), (int)(listX + 16), (int)(ry + 28), 11, pCol);

        // Mortgaged
        if (prop.mortgaged)
            DrawText("GADAI", (int)(listX + listW - 80), (int)(ry + 8), 11, RED);

        // Sewa
        std::string rentStr = "Sewa: M" + std::to_string(prop.rentL0);
        int rw = MeasureText(rentStr.c_str(), 10);
        DrawText(rentStr.c_str(), (int)(listX + listW - rw - 8), (int)(ry + 28), 10, {100, 220, 100, 255});
    }
    EndScissorMode();

    if (owned.empty())
    {
        const char *msg = "Belum memiliki properti.";
        int mw = MeasureText(msg, 13);
        DrawText(msg, (int)(listX + listW / 2 - mw / 2), (int)(listY + listH / 2 - 8), 13, {130, 130, 160, 255});
    }

    if (totalH > listH)
    {
        float sbH = listH * (listH / totalH);
        float sbY = listY + (propertiPopup.scrollY / totalH) * listH;
        DrawRectangle((int)(listX + listW - 5), (int)listY, 4, (int)listH, {40, 42, 58, 255});
        DrawRectangle((int)(listX + listW - 5), (int)sbY, 4, (int)sbH, {120, 120, 200, 255});
    }
}