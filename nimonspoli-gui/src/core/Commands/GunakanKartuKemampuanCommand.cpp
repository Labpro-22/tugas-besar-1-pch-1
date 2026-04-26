#include "GunakanKartuKemampuanCommand.hpp"

#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"
#include "../Card/SkillCard.hpp"
#include "../Exceptions/SkillCardExceptions.hpp"

GunakanKartuKemampuanCommand::GunakanKartuKemampuanCommand(int cardIndex)
    : cardIndex(cardIndex)
{
}

void GunakanKartuKemampuanCommand::execute(GameMaster &gm)
{
    GameState &gs = gm.getState();
    Player *player = gs.getCurrPlayer();

    if (player == nullptr)
    {
        throw SkillCardOverflowNotFoundException();
    }

    // if (gs.getPhase() != GamePhase::PLAYER_TURN)
    // {
    //     throw SkillCardInvalidActionException(
    //         "Kartu kemampuan hanya dapat digunakan pada fase giliran pemain.");
    // }

    // if (gs.getHasRolled())
    // {
    //     throw SkillCardInvalidActionException(
    //         "Kartu kemampuan hanya bisa digunakan sebelum melempar dadu.");
    // }

    // if (gs.getHasUsedCard())
    // {
    //     throw SkillCardInvalidActionException(
    //         "Kamu sudah menggunakan kartu kemampuan pada giliran ini.");
    // }

    const std::vector<SkillCard *> &hand = player->getHand();

    if (hand.empty())
    {
        throw SkillCardInvalidActionException(
            "Kamu tidak memiliki kartu kemampuan.");
    }

    if (cardIndex < 0 || cardIndex >= static_cast<int>(hand.size()))
    {
        throw InvalidSkillCardIndexException(cardIndex);
    }

    SkillCard *selectedCard = hand[cardIndex];

    if (selectedCard == nullptr)
    {
        throw SkillCardDiscardFailedException();
    }

    gm.useSkillCard(player, selectedCard, gs);
}