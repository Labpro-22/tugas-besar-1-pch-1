#include "GameScreen.hpp"
#include "../../../lib/raylib/include/raylib.h"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/GameState/GameState.hpp"
#include "../../core/Commands/CetakAktaCommand.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

static std::string toUpperInput(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c)
                   {
                       return static_cast<char>(std::toupper(c));
                   });

    return text;
}

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

static bool isCommandErrorOutput(const std::string &text)
{
    return text.find("tidak ditemukan") != std::string::npos ||
           text.find("bukan properti") != std::string::npos ||
           text.find("Masukkan kode") != std::string::npos ||
           text.find("tidak tersedia") != std::string::npos ||
           text.find("Game tidak dalam mode REAL") != std::string::npos;
}

static void removeTrailingNewline(std::string &text)
{
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r'))
    {
        text.pop_back();
    }
}

static Color getAktaLineColor(const std::string &line)
{
    if (line.empty())
    {
        return {185, 185, 205, 255};
    }

    if (line.find(':') != std::string::npos)
    {
        return {220, 220, 235, 255};
    }

    return {185, 185, 205, 255};
}

static void drawAlignedAktaLine(
    const std::string &line,
    float x,
    float y,
    float colonX,
    int fontSize,
    Color color)
{
    if (line.empty())
    {
        return;
    }

    size_t colonPos = line.find(':');

    if (colonPos == std::string::npos)
    {
        DrawText(line.c_str(), (int)x, (int)y, fontSize, color);
        return;
    }

    std::string label = line.substr(0, colonPos);
    std::string value = line.substr(colonPos + 1);

    DrawText(label.c_str(), (int)x, (int)y, fontSize, color);
    DrawText(":", (int)colonX, (int)y, fontSize, color);
    DrawText(value.c_str(), (int)(colonX + 18), (int)y, fontSize, color);
}

void GameScreen::triggerAktaDialog()
{
    aktaDialog.visible = true;
    aktaDialog.inputCode = "";
    aktaDialog.content = "";
    aktaDialog.propName = "";
    aktaDialog.scrollY = 0.f;
    aktaDialog.inputFocused = true;
    aktaDialog.errorMsg = "";
}

void GameScreen::searchAndFillAkta()
{
    aktaDialog.errorMsg = "";
    aktaDialog.content = "";
    aktaDialog.propName = "";
    aktaDialog.scrollY = 0.f;

    std::string code = toUpperInput(aktaDialog.inputCode);
    aktaDialog.inputCode = code;

    if (!isRealMode() || !guiManager || !guiManager->getGameMaster())
    {
        aktaDialog.errorMsg = "Game tidak dalam mode REAL.";
        return;
    }

    GameMaster *gm = guiManager->getGameMaster();

    CetakAktaCommand cmd(code);
    std::string output = cmd.getOutput(*gm);

    if (isCommandErrorOutput(output))
    {
        removeTrailingNewline(output);
        aktaDialog.errorMsg = output;
        aktaDialog.content = "";
        aktaDialog.propName = "";
        return;
    }

    aktaDialog.propName = code;
    aktaDialog.content = output;
}

void GameScreen::drawAktaDialog()
{
    if (!aktaDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 160});

    constexpr float PW = 560.f;
    constexpr float PH = 560.f;

    float px = SCREEN_W / 2.f - PW / 2.f;
    float py = SCREEN_H / 2.f - PH / 2.f;

        Color headerColor = {150, 115, 35, 255}; // kuning/emas tua
    // Color headerLine = {220, 175, 65, 255};  // kuning lebih terang

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {18, 20, 30, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.f, headerColor);
    DrawRectangle((int)px, (int)py, (int)PW, 56, headerColor);

    // garis bawah tipis, bukan strip kiri
    DrawRectangle((int)px, (int)(py + 54), (int)PW, 2, headerColor);

    std::string title = aktaDialog.propName.empty()
                            ? "AKTA PROPERTI"
                            : "AKTA - " + aktaDialog.propName;

    int titleW = MeasureText(title.c_str(), 14);
    DrawText(
        title.c_str(),
        (int)(px + PW / 2 - titleW / 2),
        (int)(py + 12),
        14,
        WHITE);

    std::string subtitle = "Masukkan kode petak untuk melihat akta";
    int subtitleW = MeasureText(subtitle.c_str(), 11);

    DrawText(
        subtitle.c_str(),
        (int)(px + PW / 2 - subtitleW / 2),
        (int)(py + 33),
        11,
        {245, 235, 200, 255});

    Rectangle xBtn = {px + PW - 36, py + 10, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);

    DrawRectangleRec(
        xBtn,
        xHov ? Color{190, 45, 45, 255} : Color{150, 35, 35, 255});

    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
    {
        aktaDialog.visible = false;
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        aktaDialog.visible = false;
        return;
    }

    float iy = py + 70;

    DrawText("Kode Petak:", (int)(px + 16), (int)(iy + 8), 12, {180, 180, 210, 255});

    Rectangle inputBox = {px + 120, iy, PW - 200, 36};
    bool inputHov = CheckCollisionPointRec(GetMousePosition(), inputBox);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        aktaDialog.inputFocused = inputHov;
    }

    DrawRectangleRec(
        inputBox,
        aktaDialog.inputFocused ? Color{35, 38, 58, 255} : Color{25, 27, 42, 255});

    DrawRectangleLinesEx(
        inputBox,
        1.5f,
        aktaDialog.inputFocused ? Color{180, 150, 60, 255} : Color{80, 80, 110, 255});

    if (aktaDialog.inputFocused)
    {
        if (IsKeyPressed(KEY_BACKSPACE) && !aktaDialog.inputCode.empty())
        {
            aktaDialog.inputCode.pop_back();
        }

        int ch = GetCharPressed();

        while (ch > 0)
        {
            if (aktaDialog.inputCode.size() < 8 && std::isalnum(static_cast<unsigned char>(ch)))
            {
                aktaDialog.inputCode += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            }

            ch = GetCharPressed();
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            searchAndFillAkta();
        }
    }

    std::string disp = aktaDialog.inputCode.empty() ? "contoh: JKT" : aktaDialog.inputCode;
    Color textCol = aktaDialog.inputCode.empty() ? Color{80, 80, 110, 255} : WHITE;

    DrawText(disp.c_str(), (int)(inputBox.x + 10), (int)(inputBox.y + 10), 14, textCol);

    if (aktaDialog.inputFocused &&
        ((int)(GetTime() * 2) % 2 == 0) &&
        !aktaDialog.inputCode.empty())
    {
        int cursorX = MeasureText(aktaDialog.inputCode.c_str(), 14);
        DrawText("|", (int)(inputBox.x + 12 + cursorX), (int)(inputBox.y + 10), 14, {180, 150, 60, 255});
    }

    Rectangle cariBtn = {px + PW - 76, iy, 62, 36};
    bool cariHov = CheckCollisionPointRec(GetMousePosition(), cariBtn);

    DrawRectangleRec(cariBtn, cariHov ? Color{60, 140, 80, 255} : Color{40, 100, 60, 255});
    DrawRectangleLinesEx(cariBtn, 1, {80, 200, 110, 255});

    int cariW = MeasureText("CARI", 12);
    DrawText("CARI", (int)(cariBtn.x + 31 - cariW / 2), (int)(cariBtn.y + 11), 12, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && cariHov)
    {
        aktaDialog.inputFocused = false;
        searchAndFillAkta();
    }

    if (!aktaDialog.errorMsg.empty())
    {
        DrawText(
            aktaDialog.errorMsg.c_str(),
            (int)(px + 16),
            (int)(iy + 44),
            11,
            {220, 80, 80, 255});
    }

    float listY = py + 128;
    float listH = PH - 142;
    float listX = px + 16;
    float listW = PW - 32;

    DrawRectangleRounded(
        {listX, listY, listW, listH},
        0.02f,
        8,
        {14, 16, 26, 255});

    if (aktaDialog.content.empty() && aktaDialog.errorMsg.empty())
    {
        const char *hint1 = "Masukkan kode petak lalu tekan ENTER atau klik CARI.";
        const char *hint2 = "Contoh kode: JKT, BDG, IKN, GBR, PLN";

        int h1w = MeasureText(hint1, 11);
        int h2w = MeasureText(hint2, 11);

        DrawText(
            hint1,
            (int)(listX + listW / 2 - h1w / 2),
            (int)(listY + listH / 2 - 18),
            11,
            {100, 100, 140, 255});

        DrawText(
            hint2,
            (int)(listX + listW / 2 - h2w / 2),
            (int)(listY + listH / 2 + 2),
            11,
            {80, 80, 110, 255});

        return;
    }

    std::vector<std::string> lines = splitLines(aktaDialog.content);

    constexpr float ROW_H = 17.f;
    constexpr float BLANK_ROW_H = 12.f;

    float totalH = 0.f;

    for (const std::string &line : lines)
    {
        totalH += line.empty() ? BLANK_ROW_H : ROW_H;
    }

    Rectangle listArea = {listX, listY, listW, listH};

    if (CheckCollisionPointRec(GetMousePosition(), listArea))
    {
        aktaDialog.scrollY -= GetMouseWheelMove() * 36.f;

        aktaDialog.scrollY =
            std::max(0.f, std::min(aktaDialog.scrollY, std::max(0.f, totalH - listH)));
    }

    BeginScissorMode((int)listX, (int)listY, (int)listW, (int)listH);

    float currentY = listY - aktaDialog.scrollY;
    const float labelX = listX + 10.f;
    const float colonX = listX + 170.f;

    for (int i = 0; i < static_cast<int>(lines.size()); i++)
    {
        float rowH = lines[i].empty() ? BLANK_ROW_H : ROW_H;
        float ry = currentY;

        currentY += rowH;

        if (ry + rowH < listY || ry > listY + listH)
        {
            continue;
        }

        if (lines[i].empty())
        {
            continue;
        }

        Color c = getAktaLineColor(lines[i]);

        drawAlignedAktaLine(
            lines[i],
            labelX,
            ry + 8,
            colonX,
            11,
            c);
    }

    EndScissorMode();

    if (totalH > listH)
    {
        float sbTrackH = listH;
        float sbH = listH * (listH / totalH);
        float maxScroll = std::max(1.f, totalH - listH);
        float sbY = listY + (aktaDialog.scrollY / maxScroll) * (sbTrackH - sbH);

        DrawRectangle((int)(listX + listW - 5), (int)listY, 4, (int)listH, {40, 42, 58, 255});
        DrawRectangle((int)(listX + listW - 5), (int)sbY, 4, (int)sbH, {200, 160, 40, 255});
    }
}