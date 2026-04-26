#ifndef CARDDECK_HPP
#define CARDDECK_HPP

#include <vector>
#include <algorithm>
#include <random>

// CardDeck<T> menyimpan pointer T* agar mendukung polymorphism
// Contoh pemakaian:
//   CardDeck<Card>  → deck Kesempatan & Dana Umum (via Card*)
//   CardDeck<Card>  → deck Skill Card (via Card*)
// Ownership kartu ada di luar deck (deck tidak delete pointer)

template <class T>
class CardDeck {
private:
    std::vector<T*> drawPile;
    std::vector<T*> discardPile;

public:
    CardDeck()  = default;
    ~CardDeck() = default;

    // ── Isi deck ────────────────────────────────
    // Tambahkan satu kartu ke drawPile
    void pushToDrawPile(T* card) {
        if (card) drawPile.push_back(card);
    }

    // Isi ulang seluruh deck sekaligus, lalu kocok
    void setCards(const std::vector<T*>& cards) {
        drawPile    = cards;
        discardPile.clear();
        shuffle();
    }

    // ── Operasi kartu ────────────────────────────
    // Ambil kartu teratas; jika habis, reshuffle dari discardPile dulu
    T* draw() {
        if (drawPile.empty()) {
            if (discardPile.empty()) return nullptr;
            reshuffle();
        }
        T* card = drawPile.back();
        drawPile.pop_back();
        return card;
    }

    // Kembalikan kartu ke discardPile
    void discard(T* card) {
        if (card) discardPile.push_back(card);
    }

    // Kocok drawPile secara acak
    void shuffle() {
        std::random_device rd;
        std::mt19937       rng(rd());
        std::shuffle(drawPile.begin(), drawPile.end(), rng);
    }

    // Pindahkan semua dari discardPile ke drawPile, lalu kocok
    void reshuffle() {
        for (T* card : discardPile) drawPile.push_back(card);
        discardPile.clear();
        shuffle();
    }

    // ── Status ───────────────────────────────────
    bool   isEmpty()         const { return drawPile.empty() && discardPile.empty(); }
    size_t drawPileSize()    const { return drawPile.size(); }
    size_t discardPileSize() const { return discardPile.size(); }

    // ── Getter (untuk save/load serialisasi) ─────
    const std::vector<T*>& getDrawPile()    const { return drawPile; }
    const std::vector<T*>& getDiscardPile() const { return discardPile; }

    void setCardsNoShuffle(const std::vector<T*>& cards) {
        drawPile    = cards;
        discardPile.clear();
        // tidak memanggil shuffle()
    }

    void setDiscardPileNoShuffle(const std::vector<T*>& cards) {
        discardPile = cards;
    }
};

#endif