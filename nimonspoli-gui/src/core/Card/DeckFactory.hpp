#ifndef DECK_FACTORY_HPP
#define DECK_FACTORY_HPP

#include "../Card/CardDeck.hpp"
#include "../Card/Card.hpp"

// ── Chance Cards (KSP) ──────────────────────────────────────────────────────
#include "../Card/GoToJailCard.hpp"
#include "../Card/MoveBackThreeCard.hpp"
#include "../Card/NearestStationCard.hpp"
#include "../Card/FreeFromJailCard.hpp"


// ── General Fund Cards (DNU) ────────────────────────────────────────────────
#include "../Card/BirthdayCard.hpp"
#include "../Card/DoctorFeeCard.hpp"
#include "../Card/ElectionCard.hpp"

// ── Skill Cards ─────────────────────────────────────────────────────────────
#include "../Card/MoveCard.hpp"
#include "../Card/DiscountCard.hpp"
#include "../Card/ShieldCard.hpp"
#include "../Card/TeleportCard.hpp"
#include "../Card/LassoCard.hpp"
#include "../Card/DemolitionCard.hpp"

#include <vector>

// ─────────────────────────────────────────────
//  DeckFactory
//  Buat CardDeck yang sudah terisi semua kartu
//  Header-only karena semua simple inline
//
//  OWNERSHIP: kartu-kartu yang dibuat di sini
//  harus di-delete oleh caller saat game selesai
//  Deck tidak delete pointer (sesuai desain CardDeck)
// ─────────────────────────────────────────────
class DeckFactory {
public:

    // ── Deck Kesempatan (KSP) ─────────────────────────────────────────────
    static CardDeck<Card>* createChanceDeck() {
        auto* deck = new CardDeck<Card>();
        std::vector<Card*> cards;

        cards.push_back(new GoToJailCard(
            "GoToJailCard", "Masuk penjara! Jangan lewati GO."));
        cards.push_back(new MoveToBackThreeCard(
            "MoveBackThreeCard", "Mundur 3 petak."));
        cards.push_back(new NearestStationCard(
            "NearestStationCard", "Maju ke stasiun terdekat."));
        cards.push_back(new FreeFromJailCard(
            "FreeFromJailCard", "Bebas dari penjara tanpa bayar."));

        deck->setCards(cards); // setCards sudah shuffle otomatis
        return deck;
    }

    // ── Deck Dana Umum (DNU) ──────────────────────────────────────────────
    static CardDeck<Card>* createCommunityDeck() {
        auto* deck = new CardDeck<Card>();
        std::vector<Card*> cards;

        cards.push_back(new BirthdayCard(
            "BirthdayCard", "Selamat ulang tahun! Terima M100 dari tiap pemain.", 100));
        cards.push_back(new DoctorFeeCard(
            "DoctorFeeCard", "Biaya dokter. Bayar M700 ke Bank.", 700));
        cards.push_back(new ElectionCard(
            "ElectionCard", "Kamu mau nyaleg. Bayar M200 ke tiap pemain.", 200));

        deck->setCards(cards);
        return deck;
    }

    // ── Deck Skill Card ───────────────────────────────────────────────────
    // Jumlah kartu sesuai kebutuhan game — bisa disesuaikan
    static CardDeck<Card>* createSkillDeck() {
        auto* deck = new CardDeck<Card>();
        std::vector<Card*> cards;

        // MoveCard — maju 3 langkah
        cards.push_back(new MoveCard("MoveCard", "Maju 3 petak.", false, 3));
        cards.push_back(new MoveCard("MoveCard", "Maju 5 petak.", false, 5));

        // DiscountCard — diskon sewa 30%, durasi 1 giliran
        cards.push_back(new DiscountCard(
            "DiscountCard", "Diskon sewa 30% selama 1 giliran.", false, 30.0, 1));

        // ShieldCard — kebal 1 giliran
        cards.push_back(new ShieldCard("ShieldCard", "Kebal dari sewa 1 giliran.", false, 1));

        // TeleportCard — pindah ke petak mana saja
        cards.push_back(new TeleportCard("TeleportCard", "Teleport ke petak pilihan.", false));

        // LassoCard — tarik lawan
        cards.push_back(new LassoCard("LassoCard", "Tarik pemain lawan ke posisimu.", false));

        // DemolitionCard — hancurkan bangunan lawan
        cards.push_back(new DemolitionCard(
            "DemolitionCard", "Hancurkan 1 bangunan milik lawan.", false));

        deck->setCards(cards);
        return deck;
    }

    // ── Cleanup helper ────────────────────────────────────────────────────
    // Panggil ini saat game selesai untuk delete semua kartu
    static void deleteDeckCards(CardDeck<Card>* deck) {
        if (!deck) return;
        for (auto* c : deck->getDrawPile())    delete c;
        for (auto* c : deck->getDiscardPile()) delete c;
    }
};

#endif