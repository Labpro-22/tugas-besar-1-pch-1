#ifndef DECK_FACTORY_HPP
#define DECK_FACTORY_HPP

#include "../Card/CardDeck.hpp"
#include "../Card/Card.hpp"
#include "../Card/SkillCard.hpp"

// Chance Cards / KSP
#include "../Card/GoToJailCard.hpp"
#include "../Card/MoveBackThreeCard.hpp"
#include "../Card/NearestStationCard.hpp"

// General Fund Cards / DNU
#include "../Card/BirthdayCard.hpp"
#include "../Card/DoctorFeeCard.hpp"
#include "../Card/ElectionCard.hpp"

// Skill Cards
#include "../Card/MoveCard.hpp"
#include "../Card/DiscountCard.hpp"
#include "../Card/ShieldCard.hpp"
#include "../Card/TeleportCard.hpp"
#include "../Card/LassoCard.hpp"
#include "../Card/DemolitionCard.hpp"
#include "FreeFromJailCard.hpp"

#include <vector>

class DeckFactory
{
public:
    // Deck Kesempatan / KSP
    static CardDeck<Card> *createChanceDeck()
    {
        CardDeck<Card> *deck = new CardDeck<Card>();
        std::vector<Card *> cards;

        cards.push_back(new GoToJailCard());
        cards.push_back(new MoveToBackThreeCard());
        cards.push_back(new NearestStationCard());

        deck->setCards(cards);
        return deck;
    }

    // Deck Dana Umum / DNU
    static CardDeck<Card> *createCommunityDeck()
    {
        CardDeck<Card> *deck = new CardDeck<Card>();
        std::vector<Card *> cards;

        cards.push_back(new BirthdayCard());
        cards.push_back(new DoctorFeeCard());
        cards.push_back(new ElectionCard());

        deck->setCards(cards);
        return deck;
    }

    // Deck Skill Card
    // Pakai CardDeck<SkillCard> supaya GameMaster tidak perlu dynamic_cast.
    static CardDeck<SkillCard> *createSkillDeck()
    {
        CardDeck<SkillCard> *deck = new CardDeck<SkillCard>();
        std::vector<SkillCard *> cards;

        // Sesuai spesifikasi:
        // MoveCard 4 lembar
        // DiscountCard 3 lembar
        // ShieldCard 2 lembar
        // TeleportCard 2 lembar
        // LassoCard 2 lembar
        // DemolitionCard 2 lembar

        for (int i = 0; i < 4; ++i)
        {
            cards.push_back(new MoveCard());
        }

        for (int i = 0; i < 3; ++i)
        {
            cards.push_back(new DiscountCard());
        }

        for (int i = 0; i < 2; ++i)
        {
            cards.push_back(new ShieldCard());
        }

        for (int i = 0; i < 2; ++i)
        {
            cards.push_back(new TeleportCard());
        }

        for (int i = 0; i < 2; ++i)
        {
            cards.push_back(new LassoCard());
        }

        for (int i = 0; i < 2; ++i)
        {
            cards.push_back(new DemolitionCard());
        }

        for (int i = 0; i < 2; ++i)
        {
            cards.push_back(new FreeFromJailCard());
        }

        deck->setCards(cards);
        return deck;
    }

    // Cleanup helper generic.
    // Bisa dipakai untuk CardDeck<Card> dan CardDeck<SkillCard>.
    template <typename T>
    static void deleteDeckCards(CardDeck<T> *deck)
    {
        if (!deck)
            return;

        for (T *card : deck->getDrawPile())
        {
            delete card;
        }

        for (T *card : deck->getDiscardPile())
        {
            delete card;
        }
    }
};

#endif