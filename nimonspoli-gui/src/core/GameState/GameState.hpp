#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <vector>
#include <string>

// Forward declarations — hindari circular dependency
class Board;
class Player;
class Bank;
class Dice;
class AuctionManager;
class Card;
class TransactionLogger;

// CardDeck adalah template class — tidak bisa forward declare, harus include
#include "../Card/CardDeck.hpp"

// ─────────────────────────────────────────────
//  Enum fase permainan
// ─────────────────────────────────────────────
enum class GamePhase {
    NOT_STARTED,
    PLAYER_TURN,        // menunggu input pemain (sebelum lempar dadu)
    DICE_ROLLED,        // sudah lempar dadu, menunggu resolusi petak
    AWAITING_BUY,       // menunggu keputusan beli properti
    AUCTION,            // lelang sedang berjalan
    BANKRUPTCY,         // proses likuidasi/kebangkrutan
    GAME_OVER           // permainan selesai
};

// ─────────────────────────────────────────────
//  GameState: snapshot seluruh data permainan
//  Diakses oleh GameMaster dan semua Command
// ─────────────────────────────────────────────
class GameState {
private:
    // ── Turn & fase ─────────────────────────────
    int         currTurn;
    int         maxTurn;
    GamePhase   phase;

    // ── Pemain ──────────────────────────────────
    std::vector<Player*> listPlayer;    // urutan giliran
    int                  currPlayerIdx; // indeks pemain aktif di listPlayer
    bool                 hasExtraTurn;  // flag double dadu
    bool                 hasRolled;     // sudah lempar dadu giliran ini?
    bool                 hasUsedCard;   // sudah pakai skill card giliran ini?

    // ── Entitas inti ────────────────────────────
    Board*          gameBoard;
    Bank*           gameBank;
    Dice*           gameDice;
    AuctionManager* auctionManager;
    CardDeck<Card>*    chanceCardDeck;
    CardDeck<Card>*    communityCardDeck;
    CardDeck<Card>*    skillCardDeck;
    TransactionLogger* logger;

public:
    // ── Konstruktor & destruktor ─────────────────
    GameState();
    GameState(
        int maxTurn,
        std::vector<Player*> listPlayer,
        Board*              board,
        Bank*               bank,
        Dice*               dice,
        AuctionManager*     auctionMgr,
        CardDeck<Card>*     chanceDeck,
        CardDeck<Card>*     communityDeck,
        CardDeck<Card>*     skillDeck,
        TransactionLogger*  logger
    );
    ~GameState() = default;

    // ── Getter: turn & fase ─────────────────────
    int       getCurrTurn()  const;
    int       getMaxTurn()   const;
    GamePhase getPhase()     const;
    bool      isGameOver()   const;

    // ── Getter: pemain ───────────────────────────
    Player*              getCurrPlayer()  const;   // pemain aktif saat ini
    int                  getCurrPlayerIdx() const;
    std::vector<Player*> getPlayers()    const;
    std::vector<Player*> getActivePlayers() const; // exclude BANKRUPT
    Player*              getPlayerById(const std::string& id) const;
    int                  getPlayerCount() const;
    bool                 getHasExtraTurn() const;
    bool                 getHasRolled()    const;
    bool                 getHasUsedCard()  const;

    // ── Getter: entitas ──────────────────────────
    Board*             getBoard()         const;
    Bank*              getBank()          const;
    Dice*              getDice()          const;
    AuctionManager*    getAuctionManager() const;
    CardDeck<Card>*    getChanceDeck()    const;
    CardDeck<Card>*    getCommunityDeck() const;
    CardDeck<Card>*    getSkillDeck()     const;
    TransactionLogger* getLogger()        const;

    // ── Setter: turn & fase ─────────────────────
    void setPhase(GamePhase p);
    void setHasExtraTurn(bool val);
    void setHasRolled(bool val);
    void setHasUsedCard(bool val);

    // ── Setter: navigasi giliran ─────────────────
    void advanceTurn();         // naikkan currTurn, reset flag giliran
    void nextPlayer();          // pindah ke pemain aktif berikutnya
    void removePlayer(Player* p); // keluarkan pemain bankrut dari urutan

    // ── Helper ───────────────────────────────────
    bool isMaxTurnReached() const;
    int  countActivePlayers() const;
};

#endif