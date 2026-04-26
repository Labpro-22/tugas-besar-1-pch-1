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
class GameMaster;
class Property;
class SkillCard;
class PropertyTile;

// CardDeck adalah template class — tidak bisa forward declare, harus include
#include "../Card/CardDeck.hpp"
#include "../utils/ConfigLoader.hpp"

// ─────────────────────────────────────────────
//  Enum fase permainan
// ─────────────────────────────────────────────
enum class GamePhase
{
    NOT_STARTED,
    PLAYER_TURN,       // menunggu input pemain (sebelum lempar dadu)
    DICE_ROLLED,       // sudah lempar dadu, menunggu resolusi petak
    AWAITING_BUY,      // menunggu keputusan beli properti
    AWAITING_TAX,      // menunggu pilihan user untuk PPH (flat vs persen)
    AWAITING_PBM,      // menunggu user dismiss dialog PBM (sudah dieksekusi)
    AWAITING_RENT,     // menunggu user klik BAYAR di dialog sewa
    AWAITING_FESTIVAL, // menunggu user pilih properti festival
    AWAITING_JAIL,
    AWAITING_DROP_SKILL_CARD,
    SHOW_CARD,  // menampilkan hasil kartu Kesempatan/Dana Umum
    AUCTION,    // lelang sedang berjalan
    BANKRUPTCY, // proses likuidasi/kebangkrutan
    GAME_OVER   // permainan selesai
};

// ─────────────────────────────────────────────
//  GameState: snapshot seluruh data permainan
//  Diakses oleh GameMaster dan semua Command
// ─────────────────────────────────────────────
class GameState
{
public:
    // ── Tipe publik ──────────────────────────────
    // Antrian pembayaran multi-pemain (ElectionCard / BirthdayCard)
    class PendingPayment
    {
    public:
        Player *debtor;   // yang harus bayar
        Player *creditor; // yang menerima (nullptr = Bank)
        int amount;
    };

private:
    // ── Turn & fase ─────────────────────────────
    int currTurn;
    int maxTurn;
    GamePhase phase;

    // ── Pemain ──────────────────────────────────
    std::vector<Player *> listPlayer; // urutan giliran
    int currPlayerIdx;                // indeks pemain aktif di listPlayer
    bool hasExtraTurn;                // flag double dadu
    bool hasRolled;                   // sudah lempar dadu giliran ini?
    bool hasUsedCard;                 // sudah pakai skill card giliran ini?

    // ── Entitas inti ────────────────────────────
    Board *gameBoard;
    Bank *gameBank;
    Dice *gameDice;
    AuctionManager *auctionManager;
    CardDeck<Card> *chanceCardDeck;
    CardDeck<Card> *communityCardDeck;
    CardDeck<SkillCard> *skillCardDeck;
    TransactionLogger *logger;
    GameMaster *gameMaster;
    TaxConfig taxcfg;

    // ── Pending data untuk GUI dialog ────────────
    // Untuk SHOW_CARD (CardDialog)
    std::string pendingCardDesc; // deskripsi kartu yang ditarik
    std::string pendingCardDeck; // "Kesempatan" atau "Dana Umum"

    Player *pendingSkillDropPlayer = nullptr;
    std::string pendingSkillDropMessage;

    // Untuk AWAITING_TAX (TaxDialog — PPH saja)
    int pendingPphFlat = 0; // jumlah flat M
    int pendingPphPct = 0;  // persentase %

    // Untuk AWAITING_PBM (PbmDialog — info saja, sudah dieksekusi)
    int pendingPbmAmount = 0; // jumlah yang sudah dibayar (info display)

    // Untuk AWAITING_RENT (SewaDialog)
    PropertyTile *pendingRentTile = nullptr; // tile yang menghasilkan sewa
    int pendingRentDice = 0;                 // nilai dadu saat landing
    int pendingRentAmount = 0;               // nilai sewa yang dihitung

    // Untuk BANKRUPTCY (LikuidasiDialog)
    int pendingDebt = 0;                         // jumlah hutang yang harus dibayar
    Player *pendingDebtor = nullptr;             // pemain yang berhutang
    Player *pendingCreditor = nullptr;           // kreditur (nullptr = Bank)
    std::vector<Property *> pendingAuctionQueue; // antrian lelang properti bangkrut ke Bank

    // Antrian pembayaran multi-pemain (private storage)
    std::vector<PendingPayment> pendingPaymentQueue;

public:
    // ── Konstruktor & destruktor ─────────────────
    GameState();
    GameState(
        int maxTurn,
        std::vector<Player *> listPlayer,
        Board *board,
        Bank *bank,
        Dice *dice,
        AuctionManager *auctionMgr,
        CardDeck<Card> *chanceDeck,
        CardDeck<Card> *communityDeck,
        CardDeck<SkillCard> *skillDeck,
        TransactionLogger *logger,
        TaxConfig taxcfg);
    ~GameState() = default;

    // ── Getter: turn & fase ─────────────────────
    int getCurrTurn() const;
    int getMaxTurn() const;
    GamePhase getPhase() const;
    bool isGameOver() const;

    // ── Getter: pemain ───────────────────────────
    Player *getCurrPlayer() const; // pemain aktif saat ini
    int getCurrPlayerIdx() const;
    std::vector<Player *> getPlayers() const;
    std::vector<Player *> getActivePlayers() const; // exclude BANKRUPT
    Player *getPlayerById(const std::string &id) const;
    int getPlayerCount() const;
    bool getHasExtraTurn() const;
    bool getHasRolled() const;
    bool getHasUsedCard() const;

    // ── Getter: entitas ──────────────────────────
    Board *getBoard() const;
    Property *getPropertyByCode(const std::string &code) const;
    Bank *getBank() const;
    Dice *getDice() const;
    AuctionManager *getAuctionManager() const;
    CardDeck<Card> *getChanceDeck() const;
    CardDeck<Card> *getCommunityDeck() const;
    CardDeck<SkillCard> *getSkillDeck() const;
    TransactionLogger *getLogger() const;
    GameMaster *getGameMaster() const;
    TaxConfig getTaxConfig() const;

    // ── Setter: turn & fase ─────────────────────
    void setPhase(GamePhase p);
    void setHasExtraTurn(bool val);
    void setHasRolled(bool val);
    void setHasUsedCard(bool val);

    // ── Setter: navigasi giliran ─────────────────
    void advanceTurn();           // naikkan currTurn, reset flag giliran
    void nextPlayer();            // pindah ke pemain aktif berikutnya
    void removePlayer(Player *p); // keluarkan pemain bankrut dari urutan
    void setCurrTurn(int t);
    void setCurrPlayerIdx(int idx);
    void setMaxTurn(int m);
    void setGameMaster(GameMaster *gm);

    // ── Helper ───────────────────────────────────
    bool isMaxTurnReached() const;
    int countActivePlayers() const;
    Player *getPendingSkillDropPlayer() const;
    const std::string &getPendingSkillDropMessage() const;

    void setPendingSkillDrop(Player *p, const std::string &msg);
    void clearPendingSkillDrop();

    // ── Getter/Setter pending dialog data ────────
    const std::string &getPendingCardDesc() const { return pendingCardDesc; }
    const std::string &getPendingCardDeck() const { return pendingCardDeck; }
    int getPendingPphFlat() const { return pendingPphFlat; }
    int getPendingPphPct() const { return pendingPphPct; }
    int getPendingPbmAmount() const { return pendingPbmAmount; }

    void setPendingCardDesc(const std::string &s) { pendingCardDesc = s; }
    void setPendingCardDeck(const std::string &s) { pendingCardDeck = s; }
    void setPendingPphFlat(int v) { pendingPphFlat = v; }
    void setPendingPphPct(int v) { pendingPphPct = v; }
    void setPendingPbmAmount(int v) { pendingPbmAmount = v; }

    // ── Getter/Setter pending rent (AWAITING_RENT) ──
    PropertyTile *getPendingRentTile() const { return pendingRentTile; }
    int getPendingRentDice() const { return pendingRentDice; }
    int getPendingRentAmount() const { return pendingRentAmount; }

    void setPendingRentTile(PropertyTile *t) { pendingRentTile = t; }
    void setPendingRentDice(int d) { pendingRentDice = d; }
    void setPendingRentAmount(int a) { pendingRentAmount = a; }
    void clearPendingRent()
    {
        pendingRentTile = nullptr;
        pendingRentDice = 0;
        pendingRentAmount = 0;
    }

    // ── Getter/Setter bankruptcy context ─────────
    int getPendingDebt() const { return pendingDebt; }
    Player *getPendingDebtor() const { return pendingDebtor; }
    Player *getPendingCreditor() const { return pendingCreditor; }
    const std::vector<Property *> &getPendingAuctionQueue() const { return pendingAuctionQueue; }

    void setPendingDebt(int d) { pendingDebt = d; }
    void setPendingDebtor(Player *p) { pendingDebtor = p; }
    void setPendingCreditor(Player *p) { pendingCreditor = p; }
    void addToPendingAuctionQueue(Property *p) { pendingAuctionQueue.push_back(p); }
    void clearPendingAuctionQueue() { pendingAuctionQueue.clear(); }

    // Multi-player payment queue
    void addToPendingPaymentQueue(Player *debtor, Player *creditor, int amount)
    {
        pendingPaymentQueue.push_back({debtor, creditor, amount});
    }
    void clearPendingPaymentQueue() { pendingPaymentQueue.clear(); }
    bool hasPendingPayment() const { return !pendingPaymentQueue.empty(); }
    PendingPayment popPendingPayment()
    {
        PendingPayment p = pendingPaymentQueue.front();
        pendingPaymentQueue.erase(pendingPaymentQueue.begin());
        return p;
    }
    const std::vector<PendingPayment> &getPendingPaymentQueue() const { return pendingPaymentQueue; }

    // Ambil & hapus properti pertama dari antrian lelang
    Property *popPendingAuction()
    {
        if (pendingAuctionQueue.empty())
            return nullptr;
        Property *p = pendingAuctionQueue.front();
        pendingAuctionQueue.erase(pendingAuctionQueue.begin());
        return p;
    }
};

#endif