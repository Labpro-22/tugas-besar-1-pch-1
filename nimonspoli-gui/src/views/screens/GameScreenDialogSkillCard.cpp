#include "GameScreen.hpp"
#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/Card/SkillCard.hpp"
#include "../../core/Card/TeleportCard.hpp"
#include "../../core/Card/LassoCard.hpp"
#include "../../core/Card/DemolitionCard.hpp"
#include "../../core/Player/Player.hpp"
#include "../../core/Property/Property.hpp"
#include "../../core/Property/StreetProperty.hpp"
#include "../../core/Commands/GunakanKartuKemampuanCommand.hpp"

#include <exception>
#include <unordered_map>
#include <functional>
#include <string>

static std::string getSkillCardTextureKey(const std::string &type)
{
    if (type == "DemolitionCard")
        return "KKS-Demolition";
    if (type == "DiscountCard")
        return "KKS-Discount";
    if (type == "LassoCard")
        return "KKS-Lasso";
    if (type == "MoveCard")
        return "KKS-Move";
    if (type == "ShieldCard")
        return "KKS-Shield";
    if (type == "TeleportCard")
        return "KKS-Teleport";
    return "";
}

static void drawWrappedText(
    const std::string &text,
    int x,
    int y,
    int maxWidth,
    int fontSize,
    Color color)
{
    std::string line;
    std::string word;
    int curY = y;

    for (size_t i = 0; i <= text.size(); ++i)
    {
        char c = (i < text.size()) ? text[i] : ' ';

        if (c == ' ' || c == '\n' || i == text.size())
        {
            if (!word.empty())
            {
                std::string testLine = line.empty() ? word : line + " " + word;

                if (MeasureText(testLine.c_str(), fontSize) > maxWidth && !line.empty())
                {
                    DrawText(line.c_str(), x, curY, fontSize, color);
                    curY += fontSize + 6;
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
                if (!line.empty())
                {
                    DrawText(line.c_str(), x, curY, fontSize, color);
                    curY += fontSize + 6;
                    line.clear();
                }
            }
        }
        else
        {
            word += c;
        }
    }

    if (!line.empty())
    {
        DrawText(line.c_str(), x, curY, fontSize, color);
    }
}

static std::string buildSkillCardSuccessMessage(SkillCard *card)
{
    if (card == nullptr)
        return "Kartu kemampuan berhasil digunakan.";

    return card->successMessage();
}

void GameScreen::triggerSkillCardDialog()
{
    skillCardDialog.visible = true;
    skillCardDialog.hoveredIdx = -1;
    skillCardDialog.awaitingTeleportTile = false;
    skillCardDialog.awaitingLassoTarget = false;
    skillCardDialog.selectedCardIdx = -1;

    lassoTargetDialog.visible = false;
    lassoTargetDialog.selectedCardIdx = -1;
    lassoTargetDialog.errorMsg.clear();

    demolitionTargetDialog.visible = false;
    demolitionTargetDialog.selectedCardIdx = -1;
    demolitionTargetDialog.errorMsg.clear();
}

static bool discardUsedSkillCard(GameMaster *gm, Player *player, int idx)
{
    if (gm == nullptr || player == nullptr)
        return false;

    if (idx < 0 || idx >= static_cast<int>(player->getHand().size()))
        return false;

    GameState &gs = gm->getState();

    SkillCard *removed = player->discardSkillCard(idx);

    if (removed != nullptr && gs.getSkillDeck() != nullptr)
    {
        gs.getSkillDeck()->discard(removed);
    }

    gs.setHasUsedCard(true);

    return true;
}

bool GameScreen::hasBlockingTileEffectDialog() const
{
    return cardDialog.visible ||
           buyDialog.visible ||
           auctionDialog.visible ||
           taxDialog.visible ||
           pbmDialog.visible ||
           festivalDialog.visible ||
           sewaDialog.visible ||
           jailDialog.visible ||
           bankruptcyDialog.visible;
}

void GameScreen::drawSkillCardDialog()
{
    if (!skillCardDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 170});

    const float PW = 500.f;
    const float PH = 400.f;
    const float px = SCREEN_W / 2.f - PW / 2.f;
    const float py = SCREEN_H / 2.f - PH / 2.f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 1.5f, {80, 80, 130, 255});

    DrawRectangle((int)px, (int)py, (int)PW, 44, {32, 34, 52, 255});
    DrawText("GUNAKAN KARTU SKILL", (int)(px + 16), (int)(py + 14), 16, {180, 180, 230, 255});

    Rectangle xBtn = {px + PW - 36, py + 8, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);

    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && xHov)
    {
        skillCardDialog.visible = false;
        return;
    }

    if (!isRealMode() || !guiManager || !guiManager->getGameMaster())
    {
        DrawText("Game tidak dalam mode REAL.", (int)px + 20, (int)py + 80, 14, {150, 150, 150, 255});
        return;
    }

    GameMaster *gm = guiManager->getGameMaster();
    Player *cur = gm->getState().getCurrPlayer();

    if (!cur)
    {
        DrawText("Tidak ada pemain aktif.", (int)px + 20, (int)py + 80, 14, {150, 150, 150, 255});
        return;
    }

    const auto &hand = cur->getHand();

    if (hand.empty())
    {
        DrawText("Kamu tidak memiliki kartu skill.", (int)px + 20, (int)py + 80, 14, {150, 150, 150, 255});
        return;
    }

    auto executeSkillCardImmediately = [this, gm](SkillCard *selectedCard, int cardIdx)
    {
        if (selectedCard == nullptr)
            return;

        try
        {
            std::string successTitle = selectedCard->getType();

            GunakanKartuKemampuanCommand cmd(cardIdx);
            cmd.execute(*gm);

            std::string successMessage = buildSkillCardSuccessMessage(selectedCard);

            skillCardDialog.visible = false;
            if (!hasBlockingTileEffectDialog())
            {
                skillCardResultDialog.visible = true;
                skillCardResultDialog.title = successTitle;
                skillCardResultDialog.message = successMessage;
            }
            pendingTileEffectAfterCard = true;
        }
        catch (const std::exception &e)
        {
            skillCardDialog.visible = false;

            skillCardResultDialog.visible = true;
            skillCardResultDialog.title = "Gagal Menggunakan Kartu";
            skillCardResultDialog.message = e.what();
        }
    };

    using Handler = std::function<void(SkillCard *, int)>;

    const std::unordered_map<std::string, Handler> popupHandlers = {
        {"TeleportCard",
         [this](SkillCard *selectedCard, int cardIdx)
         {
             skillCardDialog.awaitingTeleportTile = true;
             skillCardDialog.selectedCardIdx = cardIdx;
             skillCardDialog.visible = false;

             skillTargetHint.visible = true;
             skillTargetHint.message = selectedCard ? selectedCard->successMessage() : "Pilih petak tujuan.";
         }},
        {"LassoCard",
         [this](SkillCard *, int cardIdx)
         {
             skillCardDialog.visible = false;

             lassoTargetDialog.visible = true;
             lassoTargetDialog.selectedCardIdx = cardIdx;
             lassoTargetDialog.errorMsg.clear();
         }},
        {"DemolitionCard",
         [this](SkillCard *, int cardIdx)
         {
             skillCardDialog.visible = false;

             demolitionTargetDialog.visible = true;
             demolitionTargetDialog.selectedCardIdx = cardIdx;
             demolitionTargetDialog.errorMsg.clear();
         }}};

    const float listY = py + 64.f;

    for (int i = 0; i < static_cast<int>(hand.size()); ++i)
    {
        const float ROW_H = 64.f;
        const float ROW_GAP = 8.f;

        float rowY = listY + i * (ROW_H + ROW_GAP);

        if (rowY + ROW_H > py + PH - 16)
            break;

        Rectangle row = {px + 16, rowY, PW - 32, ROW_H};
        bool hover = CheckCollisionPointRec(GetMousePosition(), row);

        DrawRectangleRec(row, hover ? Color{45, 50, 75, 255} : Color{30, 32, 48, 255});
        DrawRectangleLinesEx(row, 1.0f, {75, 80, 115, 255});

        SkillCard *card = hand[i];
        std::string type = card ? card->getType() : "UnknownCard";
        std::string desc = card ? card->getDescription() : "";

        int titleFont = 14;
        int descFont = 11;
        int gap = 6;


        constexpr float THUMB = 36.f;
        float thumbX = row.x + 4.f;
        float thumbY = row.y + (row.height - THUMB) / 2.f;

        std::string sTexKey = getSkillCardTextureKey(type);
        if (!sTexKey.empty() && cardTextures.count(sTexKey))
        {
            Texture2D &tex = cardTextures[sTexKey];
            float scale = THUMB / (tex.width > tex.height ? tex.width : tex.height);
            float dw = tex.width * scale;
            float dh = tex.height * scale;
            DrawTextureEx(tex,
                          {thumbX + (THUMB - dw) / 2.f, thumbY + (THUMB - dh) / 2.f},
                          0.f, scale, WHITE);
        }
        else
        {
            // Fallback kotak kecil berwarna
            DrawRectangle((int)thumbX, (int)thumbY,
                          (int)THUMB, (int)THUMB, {50, 55, 80, 255});
        }

        // Teks type & deskripsi (geser ke kanan setelah thumbnail)
        float labelX = row.x + THUMB + 10.f;
        float maxDescW = row.width - THUMB - 18.f;
        int blockH = titleFont + gap + descFont;
        int textY = (int)(row.y + row.height / 2 - blockH / 2 + 2);
        DrawText(type.c_str(), (int)labelX, textY, titleFont, WHITE);
        if (!desc.empty())
        {
            std::string descTrunc = desc;
            // Potong kalau terlalu panjang
            if (MeasureText(descTrunc.c_str(), descFont) > (int)maxDescW)
            {
                while (!descTrunc.empty() &&
                       MeasureText((descTrunc + "...").c_str(), descFont) > (int)maxDescW)
                    descTrunc.pop_back();
                descTrunc += "...";
            }
            DrawText(descTrunc.c_str(), (int)labelX,
                     textY + titleFont + gap, descFont, {175, 178, 205, 255});
        }

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            SkillCard *selectedCard = card;

            if (selectedCard == nullptr)
                return;

            auto it = popupHandlers.find(selectedCard->getType());

            if (it != popupHandlers.end())
            {
                it->second(selectedCard, i);
                return;
            }

            executeSkillCardImmediately(selectedCard, i);
            return;
        }
    }
}

void GameScreen::drawDemolitionTargetDialog()
{
    if (!demolitionTargetDialog.visible)
        return;

    if (!isRealMode() || !guiManager || !guiManager->getGameMaster())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *cur = gs.getCurrPlayer();

    if (cur == nullptr)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150});

    const float PW = 600.0f;
    const float PH = 440.0f;
    const float px = SCREEN_W / 2.0f - PW / 2.0f;
    const float py = SCREEN_H / 2.0f - PH / 2.0f;

    DrawRectangleRounded({px, py, PW, PH}, 0.04f, 8, {18, 20, 34, 255});
    DrawRectangleRoundedLines({px, py, PW, PH}, 0.04f, 8, {90, 95, 150, 255});

    DrawRectangleRounded({px, py, PW, 48}, 0.04f, 8, {32, 34, 52, 255});
    DrawText("PILIH PROPERTI TARGET", (int)px + 20, (int)py + 16, 16, WHITE);

    Rectangle xBtn = {px + PW - 38, py + 10, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);

    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);

    if (xHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        demolitionTargetDialog.visible = false;
        demolitionTargetDialog.selectedCardIdx = -1;
        demolitionTargetDialog.errorMsg.clear();

        skillCardDialog.visible = true;
        return;
    }

    DrawText(
        "Pilih Street milik lawan yang berstatus OWNED dan memiliki rumah/hotel.",
        (int)px + 24,
        (int)py + 58,
        12,
        {190, 195, 220, 255});

    int idx = demolitionTargetDialog.selectedCardIdx;
    const auto &hand = cur->getHand();

    if (idx < 0 || idx >= static_cast<int>(hand.size()))
    {
        DrawText("Index DemolitionCard tidak valid.", (int)px + 24, (int)py + 92, 13, {255, 150, 150, 255});
        return;
    }

    DemolitionCard *demoCard = dynamic_cast<DemolitionCard *>(hand[idx]);

    if (demoCard == nullptr)
    {
        DrawText("Kartu yang dipilih bukan DemolitionCard.", (int)px + 24, (int)py + 92, 13, {255, 150, 150, 255});
        return;
    }

    int rowIndex = 0;
    bool hasTarget = false;

    const float listY = py + 88.0f;
    const float rowH = 50.0f;
    const float rowGap = 8.0f;
    const float maxListBottom = py + PH - 70.0f;

    for (Player *owner : gs.getPlayers())
    {
        if (owner == nullptr)
            continue;

        if (owner == cur)
            continue;

        if (owner->getStatus() == PlayerStatus::BANKRUPT)
            continue;

        for (Property *prop : owner->getProperties())
        {
            if (prop == nullptr)
                continue;

            if (prop->getStatus() != PropertyStatus::OWNED)
                continue;

            StreetProperty *sp = dynamic_cast<StreetProperty *>(prop);

            if (sp == nullptr)
                continue;

            if (sp->getBuildingCount() <= 0 && !sp->gethasHotel())
                continue;

            hasTarget = true;

            float rowY = listY + rowIndex * (rowH + rowGap);

            if (rowY + rowH > maxListBottom)
                break;

            Rectangle row = {px + 24, rowY, PW - 48, rowH};
            bool hover = CheckCollisionPointRec(GetMousePosition(), row);

            DrawRectangleRounded(
                row,
                0.08f,
                8,
                hover ? Color{55, 65, 105, 255} : Color{32, 35, 54, 255});

            DrawRectangleRoundedLines(row, 0.08f, 8, {75, 80, 115, 255});

            std::string buildingInfo;

            if (sp->gethasHotel())
                buildingInfo = "Hotel";
            else
                buildingInfo = std::to_string(sp->getBuildingCount()) + " rumah";

            std::string line1 = prop->getName() + " (" + prop->getCode() + ")";
            std::string line2 = "Pemilik: " + owner->getUsername() + " | " + buildingInfo;

            DrawText(line1.c_str(), (int)row.x + 14, (int)row.y + 8, 14, WHITE);
            DrawText(line2.c_str(), (int)row.x + 14, (int)row.y + 28, 12, {190, 195, 220, 255});

            if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                try
                {
                    std::string propName = prop->getName();
                    std::string ownerName = owner->getUsername();

                    demoCard->setTargetProperty(prop->getId());

                    GunakanKartuKemampuanCommand cmd(idx);
                    cmd.execute(*gm);

                    demolitionTargetDialog.visible = false;
                    demolitionTargetDialog.selectedCardIdx = -1;
                    demolitionTargetDialog.errorMsg.clear();

                    if (!hasBlockingTileEffectDialog())
                    {
                        skillCardResultDialog.visible = true;
                        skillCardResultDialog.title = "DemolitionCard";
                        skillCardResultDialog.message =
                            "DemolitionCard berhasil digunakan pada " +
                            propName + " milik " + ownerName + ".";
                    }
                }
                catch (const std::exception &e)
                {
                    demolitionTargetDialog.errorMsg = e.what();
                }

                return;
            }

            rowIndex++;
        }
    }

    if (!hasTarget)
    {
        DrawText(
            "Tidak ada properti lawan yang bisa dihancurkan.",
            (int)px + 24,
            (int)py + 100,
            14,
            {255, 190, 190, 255});

        DrawText(
            "Syarat: properti harus Street, milik lawan, OWNED, dan punya rumah/hotel.",
            (int)px + 24,
            (int)py + 126,
            12,
            {190, 195, 220, 255});
    }

    if (!demolitionTargetDialog.errorMsg.empty())
    {
        DrawText(
            demolitionTargetDialog.errorMsg.c_str(),
            (int)px + 24,
            (int)(py + PH - 42),
            12,
            {255, 120, 120, 255});
    }
}

void GameScreen::drawSkillCardResultDialog()
{
    if (!skillCardResultDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 170});

    const float PW = 620.0f;
    const float PH = 300.0f;
    const float px = SCREEN_W / 2.0f - PW / 2.0f;
    const float py = SCREEN_H / 2.0f - PH / 2.0f;

    DrawRectangleRounded({px, py, PW, PH}, 0.04f, 8, {18, 20, 34, 255});
    DrawRectangleRoundedLines({px, py, PW, PH}, 0.04f, 8, {90, 95, 150, 255});

    DrawRectangleRounded({px, py, PW, 54}, 0.04f, 8, {34, 36, 58, 255});
    DrawText("KARTU KEMAMPUAN", (int)(px + 22), (int)(py + 18), 16, WHITE);

    DrawText(
        skillCardResultDialog.title.c_str(),
        (int)(px + 28),
        (int)(py + 82),
        22,
        {255, 230, 150, 255});

    Rectangle msgBox = {px + 28, py + 122, PW - 56, 92};
    DrawRectangleRounded(msgBox, 0.04f, 8, {30, 33, 52, 255});
    DrawRectangleRoundedLines(msgBox, 0.04f, 8, {75, 80, 120, 255});

    drawWrappedText(
        skillCardResultDialog.message,
        (int)(msgBox.x + 14),
        (int)(msgBox.y + 16),
        (int)(msgBox.width - 28),
        13,
        {230, 230, 235, 255});

    Rectangle okBtn = {px + PW / 2.0f - 60.0f, py + PH - 62.0f, 120.0f, 38.0f};
    bool hover = CheckCollisionPointRec(GetMousePosition(), okBtn);

    DrawRectangleRounded(okBtn, 0.12f, 8, hover ? Color{82, 98, 155, 255} : Color{58, 68, 115, 255});
    DrawRectangleRoundedLines(okBtn, 0.12f, 8, {120, 130, 180, 255});
    DrawText("OK", (int)(okBtn.x + 47), (int)(okBtn.y + 11), 14, WHITE);

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        skillCardResultDialog.visible = false;
        skillCardResultDialog.title.clear();
        skillCardResultDialog.message.clear();
        pendingTileEffectAfterCard = false;
    }
}

void GameScreen::handleTeleportTargetClick(int selectedTile)
{
    if (!skillCardDialog.awaitingTeleportTile)
        return;

    if (!isRealMode() || !guiManager || !guiManager->getGameMaster())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *cur = gs.getCurrPlayer();

    if (cur == nullptr || selectedTile < 0)
        return;

    int idx = skillCardDialog.selectedCardIdx;
    const auto &hand = cur->getHand();

    if (idx < 0 || idx >= static_cast<int>(hand.size()))
    {
        skillCardResultDialog.visible = true;
        skillCardResultDialog.title = "Gagal Menggunakan Kartu";
        skillCardResultDialog.message = "Index TeleportCard tidak valid.";
        return;
    }

    TeleportCard *teleportCard = dynamic_cast<TeleportCard *>(hand[idx]);

    if (teleportCard == nullptr)
    {
        skillCardResultDialog.visible = true;
        skillCardResultDialog.title = "Gagal Menggunakan Kartu";
        skillCardResultDialog.message = "Kartu yang dipilih bukan TeleportCard.";
        return;
    }

    try
    {
        teleportCard->setTargetPosition(selectedTile, gs);

        GunakanKartuKemampuanCommand cmd(idx);
        cmd.execute(*gm);

        skillCardDialog.awaitingTeleportTile = false;
        skillCardDialog.selectedCardIdx = -1;
        skillTargetHint.visible = false;

        if (!hasBlockingTileEffectDialog())
        {
            skillCardResultDialog.visible = true;
            skillCardResultDialog.title = "TeleportCard";
            skillCardResultDialog.message = "TeleportCard berhasil digunakan. Bidak dipindahkan ke petak tujuan.";
        }
    }
    catch (const std::exception &e)
    {
        skillCardDialog.awaitingTeleportTile = false;
        skillCardDialog.selectedCardIdx = -1;
        skillTargetHint.visible = false;

        skillCardResultDialog.visible = true;
        skillCardResultDialog.title = "Gagal Menggunakan Kartu";
        skillCardResultDialog.message = e.what();
    }
}

void GameScreen::drawSkillTargetHint()
{
    if (!skillTargetHint.visible)
        return;

    Rectangle bar = {340, 18, SCREEN_W - 680.0f, 44};

    DrawRectangleRounded(bar, 0.12f, 8, {22, 24, 38, 235});
    DrawRectangleRoundedLines(bar, 0.12f, 8, {120, 130, 190, 255});

    drawWrappedText(
        skillTargetHint.message,
        (int)(bar.x + 18),
        (int)(bar.y + 13),
        (int)(bar.width - 36),
        13,
        {245, 245, 245, 255});
}

void GameScreen::drawLassoTargetDialog()
{
    if (!lassoTargetDialog.visible)
        return;

    if (!isRealMode() || !guiManager || !guiManager->getGameMaster())
        return;

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();
    Player *cur = gs.getCurrPlayer();

    if (cur == nullptr)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150});

    const float PW = 460.0f;
    const float PH = 320.0f;
    const float px = SCREEN_W / 2.0f - PW / 2.0f;
    const float py = SCREEN_H / 2.0f - PH / 2.0f;

    DrawRectangleRounded({px, py, PW, PH}, 0.04f, 8, {18, 20, 34, 255});
    DrawRectangleRoundedLines({px, py, PW, PH}, 0.04f, 8, {90, 95, 150, 255});

    DrawRectangleRounded({px, py, PW, 48}, 0.04f, 8, {32, 34, 52, 255});
    DrawText("PILIH TARGET LASSO", (int)px + 20, (int)py + 16, 16, WHITE);

    Rectangle xBtn = {px + PW - 38, py + 10, 28, 28};
    bool xHov = CheckCollisionPointRec(GetMousePosition(), xBtn);

    DrawRectangleRec(xBtn, xHov ? Color{180, 60, 60, 255} : Color{110, 40, 40, 255});
    DrawText("X", (int)(xBtn.x + 9), (int)(xBtn.y + 8), 12, WHITE);

    if (xHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        lassoTargetDialog.visible = false;
        lassoTargetDialog.selectedCardIdx = -1;
        lassoTargetDialog.errorMsg.clear();

        skillCardDialog.visible = true;
        return;
    }

    int rowIndex = 0;

    for (Player *p : gs.getPlayers())
    {
        if (p == nullptr || p == cur || p->getStatus() == PlayerStatus::BANKRUPT)
            continue;
        if (p->isInJail())
            continue;
        Rectangle row = {px + 24, py + 70 + rowIndex * 48.0f, PW - 48, 38};
        bool hover = CheckCollisionPointRec(GetMousePosition(), row);

        DrawRectangleRounded(row, 0.08f, 8, hover ? Color{55, 65, 105, 255} : Color{32, 35, 54, 255});

        std::string label = p->getUsername() + " - posisi " + std::to_string(p->getPosition());
        DrawText(label.c_str(), (int)row.x + 14, (int)row.y + 11, 13, WHITE);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            int idx = lassoTargetDialog.selectedCardIdx;
            const auto &hand = cur->getHand();

            if (idx < 0 || idx >= static_cast<int>(hand.size()))
            {
                lassoTargetDialog.errorMsg = "Index LassoCard tidak valid.";
                return;
            }

            LassoCard *lassoCard = dynamic_cast<LassoCard *>(hand[idx]);

            if (lassoCard == nullptr)
            {
                lassoTargetDialog.errorMsg = "Kartu yang dipilih bukan LassoCard.";
                return;
            }

            try
            {
                lassoCard->setTargetPlayerUsername(p->getUsername());

                GunakanKartuKemampuanCommand cmd(idx);
                cmd.execute(*gm);

                lassoTargetDialog.visible = false;
                lassoTargetDialog.selectedCardIdx = -1;
                lassoTargetDialog.errorMsg.clear();

                if (!hasBlockingTileEffectDialog())
                {
                    skillCardResultDialog.visible = true;
                    skillCardResultDialog.title = "LassoCard";
                    skillCardResultDialog.message =
                        "LassoCard berhasil digunakan. " + p->getUsername() +
                        " ditarik ke petak Anda.";
                }
            }
            catch (const std::exception &e)
            {
                lassoTargetDialog.errorMsg = e.what();
            }

            return;
        }

        rowIndex++;
    }

    if (rowIndex == 0)
    {
        DrawText("Tidak ada pemain lawan yang valid.", (int)px + 24, (int)py + 80, 13, {220, 220, 220, 255});
    }

    if (!lassoTargetDialog.errorMsg.empty())
    {
        DrawText(
            lassoTargetDialog.errorMsg.c_str(),
            (int)px + 24,
            (int)(py + PH - 42),
            12,
            {255, 120, 120, 255});
    }
}