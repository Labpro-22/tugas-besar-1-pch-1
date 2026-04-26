#include "GameScreen.hpp"

#include "../../core/GameMaster/GameMaster.hpp"
#include "../../core/GameState/GameState.hpp"
#include "../../core/Player/Player.hpp"
#include "../../core/Card/SkillCard.hpp"
#include "../../core/Commands/DropKartuKemampuanCommand.hpp"

#include <string>
#include <vector>
#include <exception>

void GameScreen::triggerDropSkillCardDialog()
{
    dropSkillCardDialog.visible = true;
    dropSkillCardDialog.hoveredIdx = -1;
    dropSkillCardDialog.errorMsg.clear();
}

void GameScreen::drawDropSkillCardDialog()
{
    if (!dropSkillCardDialog.visible)
        return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 180});

    const float PW = 560.0f;
    const float PH = 420.0f;
    const float px = SCREEN_W / 2.0f - PW / 2.0f;
    const float py = SCREEN_H / 2.0f - PH / 2.0f;

    DrawRectangle((int)px, (int)py, (int)PW, (int)PH, {22, 24, 38, 255});
    DrawRectangleLinesEx({px, py, PW, PH}, 2.0f, {90, 90, 140, 255});

    DrawRectangle((int)px, (int)py, (int)PW, 48, {34, 36, 58, 255});
    DrawText("DROP KARTU KEMAMPUAN", (int)(px + 16), (int)(py + 16), 16, WHITE);

    if (!isRealMode() || guiManager == nullptr || guiManager->getGameMaster() == nullptr)
    {
        DrawText("Game tidak dalam mode REAL.", (int)px + 20, (int)py + 80, 14, {220, 220, 220, 255});
        return;
    }

    GameMaster *gm = guiManager->getGameMaster();
    GameState &gs = gm->getState();

    Player *player = gs.getPendingSkillDropPlayer();
    if (player == nullptr)
    {
        player = gs.getCurrPlayer();
    }

    if (player == nullptr)
    {
        DrawText("Tidak ada pemain yang wajib membuang kartu.", (int)px + 20, (int)py + 80, 14, {220, 220, 220, 255});
        return;
    }

    std::string subtitle =
        player->getUsername() + " memiliki " +
        std::to_string(player->getHandSize()) +
        " kartu. Pilih 1 kartu untuk dibuang.";

    DrawText(subtitle.c_str(), (int)px + 20, (int)py + 72, 14, {230, 230, 230, 255});

    if (!gs.getPendingSkillDropMessage().empty())
    {
        DrawText(gs.getPendingSkillDropMessage().c_str(), (int)px + 20, (int)py + 96, 12, {255, 215, 130, 255});
    }

    const std::vector<SkillCard *> &hand = player->getHand();

    float listY = py + 130.0f;

    for (int i = 0; i < static_cast<int>(hand.size()); ++i)
    {
        SkillCard *card = hand[i];
        if (card == nullptr)
            continue;

        float rowY = listY + i * 54.0f;
        Rectangle row = {px + 20, rowY, PW - 40, 46};

        bool hover = CheckCollisionPointRec(GetMousePosition(), row);

        DrawRectangleRec(row, hover ? Color{52, 58, 88, 255} : Color{32, 35, 54, 255});
        DrawRectangleLinesEx(row, 1.0f, {75, 80, 115, 255});

        std::string title = std::to_string(i + 1) + ". " + card->getType();
        DrawText(title.c_str(), (int)row.x + 12, (int)row.y + 8, 14, WHITE);

        std::string desc = card->getDescription();
        DrawText(desc.c_str(), (int)row.x + 12, (int)row.y + 26, 11, {190, 190, 210, 255});

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            try
            {
                std::string droppedCardName = card->getType();

                DropKartuKemampuanCommand cmd(i);
                cmd.execute(*gm);

                int remainingCards = player->getHandSize();

                dropSkillCardDialog.visible = false;
                dropSkillCardDialog.errorMsg.clear();

                skillCardResultDialog.visible = true;
                skillCardResultDialog.title = "Kartu Berhasil Dibuang";
                skillCardResultDialog.message =
                    droppedCardName + " telah dibuang. Sekarang kamu memiliki " +
                    std::to_string(remainingCards) +
                    " kartu di tangan.";
            }
            catch (const std::exception &e)
            {
                dropSkillCardDialog.errorMsg = e.what();
            }
        }
    }

    if (!dropSkillCardDialog.errorMsg.empty())
    {
        DrawText(
            dropSkillCardDialog.errorMsg.c_str(),
            (int)px + 20,
            (int)(py + PH - 34),
            12,
            {255, 120, 120, 255});
    }
}