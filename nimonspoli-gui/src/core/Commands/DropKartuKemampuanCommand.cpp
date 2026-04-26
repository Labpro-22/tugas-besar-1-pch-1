#include "DropKartuKemampuanCommand.hpp"

#include "../GameMaster/GameMaster.hpp"
#include "../GameState/GameState.hpp"
#include "../Player/Player.hpp"
#include "../Card/SkillCard.hpp"
#include "../Card/CardDeck.hpp"
#include "../Exceptions/SkillCardExceptions.hpp"

#include <string>

DropKartuKemampuanCommand::DropKartuKemampuanCommand(int cardIndex)
    : cardIndex(cardIndex)
{
}

void DropKartuKemampuanCommand::execute(GameMaster &gm)
{
    GameState &state = gm.getState();

    Player *player = state.getPendingSkillDropPlayer();

    if (player == nullptr)
    {
        player = state.getCurrPlayer();
    }

    if (player == nullptr)
    {
        throw SkillCardOverflowNotFoundException();
    }

    if (player->getHandSize() <= 3)
    {
        state.clearPendingSkillDrop();

        if (state.getPhase() == GamePhase::AWAITING_DROP_SKILL_CARD)
        {
            state.setPhase(GamePhase::PLAYER_TURN);
        }

        throw SkillCardDropNotRequiredException();
    }

    if (cardIndex < 0 || cardIndex >= player->getHandSize())
    {
        throw InvalidSkillCardIndexException(cardIndex);
    }

    SkillCard *removed = player->discardSkillCard(cardIndex);

    if (removed == nullptr)
    {
        throw SkillCardDiscardFailedException();
    }

    std::string cardName = removed->getType();

    CardDeck<SkillCard> *skillDeck = state.getSkillDeck();

    if (skillDeck != nullptr)
    {
        skillDeck->discard(removed);
    }

    gm.log(
        player->getUsername(),
        "DROP_SKILL_CARD",
        "Membuang kartu kemampuan: " + cardName);

    state.clearPendingSkillDrop();

    if (state.getPhase() == GamePhase::AWAITING_DROP_SKILL_CARD)
    {
        state.setPhase(GamePhase::PLAYER_TURN);
    }
}