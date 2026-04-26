#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/GameState/GameState.hpp"
#include "../../core/Player/Player.hpp"
#include "../../core/Commands/CetakPropertiCommand.hpp"

#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

static std::vector<std::string> splitLines(const std::string &text)
{
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;

    while (std::getline(iss, line))
    {
        lines.push_back(line);
    }

    if (lines.empty())
    {
        lines.push_back("");
    }

    return lines;
}

static std::string trimTrailingNewline(std::string text)
{
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
    {
        text.pop_back();
    }

    return text;
}

static std::string trimLeft(std::string text)
{
    while (!text.empty() && (text.front() == ' ' || text.front() == '\t'))
    {
        text.erase(text.begin());
    }

    return text;
}

static bool isEmptyPropertyContent(const std::string &content)
{
    return content.empty() ||
           content.find("belum memiliki properti") != std::string::npos ||
           content.find("Belum memiliki properti") != std::string::npos;
}

static bool isErrorContent(const std::string &content)
{
    return content.find("tidak tersedia") != std::string::npos ||
           content.find("Tidak ada pemain aktif") != std::string::npos ||
           content.find("Pemain tidak valid") != std::string::npos ||
           content.find("Gagal") != std::string::npos;
}

static Color getLineColor(const std::string &line, int playerIdx, const Color playerColors[4])
{
    std::string trimmed = trimLeft(line);

    if (trimmed.empty())
    {
        return {185, 185, 205, 255};
    }
    if (trimmed.rfind("===", 0) == 0)
    {
        return (playerIdx >= 0 && playerIdx < 4)
                   ? playerColors[playerIdx]
                   : Color{220, 220, 255, 255};
    }

    if (trimmed.front() == '[')
    {
        return {255, 230, 150, 255};
    }

    if (trimmed.front() == '-')
    {
        return {220, 220, 255, 255};
    }

    if (trimmed.rfind("Total", 0) == 0)
    {
        return {150, 220, 150, 255};
    }

    return {185, 185, 205, 255};
}

void GameScreen::triggerCetakPropertiDialog(int playerIdx)
{
    cetakPropertiDialog.scrollY = 0.f;
    cetakPropertiDialog.playerIdx = playerIdx;
    cetakPropertiDialog.visible = true;

    if (!isRealMode())
    {
        cetakPropertiDialog.visible = false;
        return;
    }

    if (!guiManager || !guiManager->getGameMaster())
    {
        cetakPropertiDialog.visible = false;
        return;
    }

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *p = gs.getCurrPlayer();

    if (!p)
    {
        cetakPropertiDialog.visible = false;
        return;
    }

    cetakPropertiDialog.playerName = p->getUsername();

    const auto &players = gs.getPlayers();
    int currentIdx = -1;

    for (int i = 0; i < static_cast<int>(players.size()); i++)
    {
        if (players[i] == p)
        {
            currentIdx = i;
            break;
        }
    }

    cetakPropertiDialog.playerIdx = currentIdx;

    CetakPropertiCommand cmd;
    cetakPropertiDialog.content = cmd.getOutput(*gm);
}

void GameScreen::drawCetakPropertiDialog()
{
    if (!cetakPropertiDialog.visible)
    {
        return;
    }

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 580.f;
    constexpr float PH = 560.f;

    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {18, 20, 30, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, {80, 130, 200, 255});

    int pidx = cetakPropertiDialog.playerIdx;

    Color hdrCol = (pidx >= 0 && pidx < 4)
                       ? Color{
                             (unsigned char)(playerColors[pidx].r / 2),
                             (unsigned char)(playerColors[pidx].g / 2),
                             (unsigned char)(playerColors[pidx].b / 2),
                             255}
                       : Color{30, 50, 80, 255};

    DrawRectangle((int)px, (int)py, (int)PW, 48, hdrCol);

    std::string title = "PROPERTI - " + cetakPropertiDialog.playerName;
    int tw = MeasureText(title.c_str(), 14);
    DrawText(title.c_str(), (int)(px + PW / 2 - tw / 2), (int)(py + 16), 14, WHITE);

    Rectangle xBtn = {px + PW - 36, py + 10, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);

    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
    {
        cetakPropertiDialog.visible = false;
        return;
    }

    float listX = px + 16;
    float listY = py + 62;
    float listW = PW - 32;
    float listH = PH - 84;

    DrawRectangleRounded(
        {listX, listY, listW, listH},
        0.02f,
        8,
        {14, 16, 26, 255});

    std::string content = cetakPropertiDialog.content;
    std::vector<std::string> lines = splitLines(content);

    bool emptyState = isEmptyPropertyContent(content);
    bool errorState = isErrorContent(content) && !emptyState;

    constexpr float ROW_H = 18.f;
    float totalH = lines.size() * ROW_H;

    Rectangle listArea = {listX, listY, listW, listH};

    if (CheckCollisionPointRec(GetMousePosition(), listArea))
    {
        cetakPropertiDialog.scrollY -= GetMouseWheelMove() * 36.f;

        float maxScroll = std::max(0.f, totalH - listH);

        cetakPropertiDialog.scrollY =
            std::max(0.f, std::min(cetakPropertiDialog.scrollY, maxScroll));
    }

    BeginScissorMode((int)listX, (int)listY, (int)listW, (int)listH);

    if (emptyState)
    {
        std::string msg = trimTrailingNewline(content);

        int mw = MeasureText(msg.c_str(), 13);

        DrawText(
            msg.c_str(),
            (int)(listX + listW / 2 - mw / 2),
            (int)(listY + listH / 2 - 8),
            13,
            {150, 150, 180, 255});
    }
    else if (errorState)
    {
        std::string msg = trimTrailingNewline(content);

        DrawText(
            msg.c_str(),
            (int)(listX + 20),
            (int)(listY + listH / 2 - 8),
            13,
            {255, 170, 170, 255});
    }
    else
    {
        for (int i = 0; i < static_cast<int>(lines.size()); i++)
        {
            float ry = listY + i * ROW_H - cetakPropertiDialog.scrollY;

            if (ry + ROW_H < listY || ry > listY + listH)
            {
                continue;
            }

            Color c = getLineColor(lines[i], pidx, playerColors);

            DrawText(lines[i].c_str(), (int)(listX + 10), (int)(ry + 8), 11, c);
        }
    }

    EndScissorMode();

    if (!emptyState && !errorState && totalH > listH)
    {
        float sbTrackH = listH;
        float sbH = listH * (listH / totalH);
        float maxScroll = std::max(1.f, totalH - listH);
        float sbY = listY + (cetakPropertiDialog.scrollY / maxScroll) * (sbTrackH - sbH);

        DrawRectangle((int)(listX + listW - 5), (int)listY, 4, (int)listH, {40, 42, 58, 255});
        DrawRectangle((int)(listX + listW - 5), (int)sbY, 4, (int)sbH, {80, 130, 200, 255});
    }
}