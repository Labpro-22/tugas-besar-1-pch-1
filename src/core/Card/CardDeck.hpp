#ifndef CARDDECK_HPP
#define CARDDECK_HPP



template <class T>
class CardDeck
{
private:
    std::vector<T> drawPile;
    std::vector<T> discardPile;

public:
    CardDeck();
    ~CardDeck();
    void setCards();
    bool isEmpty();
    std::vector<T> getDrawPile();
    std::vector<T> getDiscardPile();
    T *draw();
    void pushToDrawPile(T card);
    void discard(T card);
    void shuffle();
    void reshuffle();
    size_t drawPileSize();
    size_t discardPileSize();
};

#endif