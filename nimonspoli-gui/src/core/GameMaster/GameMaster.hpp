#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include <string>
#include <vector>
#include "../GameState/GameState.hpp"

// Forward declarations
class Player;
class Board;
class Bank;
class Dice;
class JailTile;
class Property;
class TransactionLogger;
class SkillCard;
class StreetProperty;

// ─────────────────────────────────────────────
//  GameMaster: controller utama permainan
//  Semua Command berinteraksi melalui kelas ini
// ─────────────────────────────────────────────
class GameMaster
{
private:
    GameState state;

    // ── Helper internal ──────────────────────────
    JailTile *findJailTile() const;
    int findJailIndex() const;    // indeks petak penjara di Board
    void distributeSkillCards();  // bagikan 1 kartu ke tiap pemain awal giliran
    void tickFestivalDurations(); // kurangi durasi festival semua properti pemain aktif
    void tickDiscountDurations();
    void checkWinCondition();     // evaluasi kondisi menang, set GAME_OVER jika perlu

public:
    // ── Konstruktor ──────────────────────────────
    explicit GameMaster(GameState initialState);

    // ── Akses state (read-only untuk Command) ────
    GameState &getState();
    const GameState &getState() const;

    // ── Main loop ────────────────────────────────
    void start();                                    // inisialisasi & mulai loop utama (CLI)
    void handleCommand(const std::string &rawInput); // parse & dispatch command
    void beginTurn();                                // awal giliran: bagikan kartu, set fase
    void endTurn();                                  // akhir giliran: tick festival, nextPlayer/advanceTurn

    // ─────────────────────────────────────────────
    //  Method yang dibutuhkan Command
    // ─────────────────────────────────────────────

    // Pergerakan pemain
    // Gerakkan pemain sejumlah steps, tangani GO salary jika melewati petak GO
    // Panggil tile->onLanded() di akhir
    void movePlayer(Player *player, int steps);

    // Teleport langsung ke indeks petak tertentu (untuk TeleportCard, kartu Kesempatan, dll)
    // passThroughGo = true → pemain tetap dapat gaji jika melewati GO
    void teleportPlayer(Player *player, int targetIndex, bool passThroughGo = true);

    // Penjara
    void sendPlayerToJail(Player *player); // pindahkan ke penjara + set status JAILED
    bool releaseFromJail(Player *player);  // lepas dari penjara (return true jika berhasil)

    // Kontrol giliran
    void setExtraTurn(bool val); // set flag giliran tambahan (double dadu)
    void endCurrentTurn();       // paksa akhiri giliran (masuk penjara 3× double)
    bool hasExtraTurn() const;

    // Properti
    // Tawarkan beli properti ke pemain; jika ditolak/tidak mampu → lelang
    void handlePropertyLanding(Player *player, Property *prop);

    // Jalankan sesi lelang untuk satu properti
    // triggerPlayer = pemain yang memicu lelang (tidak ikut di awal)
    void startAuction(Property *prop, Player *triggerPlayer);

    // Kebangkrutan
    // Cek apakah player mampu bayar debt; jika tidak → proses bangkrut
    // creditor = nullptr berarti bangkrut ke Bank
    int handleDebtPayment(Player *debtor, int debt, Player *creditor = nullptr);

    // Proses kebangkrutan ke pemain lain
    void handleBankruptcy(Player *from, Player *to);

    // Proses kebangkrutan ke Bank (lelang semua properti)
    void handleBankruptcy(Player *from, Bank *bank);

    // Helper likuidasi
    void sellPropertyToBank(Player* player, Property* prop);
    void mortgageProperty(Player* player, Property* prop);

    // Proses pembayaran berikutnya dari antrian kartu multi-pemain
    // (ElectionCard / BirthdayCard). Dipanggil oleh kartu saat execute()
    // dan oleh BankruptcyDialog setiap kali satu pembayaran selesai.
    void processNextCardPayment();


    // Kemenangan
    // Hitung & umumkan pemenang (dipanggil saat GAME_OVER)
    void handleWinner();

    // ─────────────────────────────────────────────
    //  Helper untuk Command: query state
    // ─────────────────────────────────────────────

    // Apakah pemain memonopoli seluruh color group?
    bool hasMonopoly(Player *player, const std::string &colorGroup) const;

    // Hitung berapa railroad yang dimiliki pemain
    int countPlayerRailroads(Player *player) const;

    // Hitung berapa utility yang dimiliki pemain
    int countPlayerUtilities(Player *player) const;

    // Cari indeks petak stasiun terdekat dari posisi saat ini
    int findNearestRailroad(int currentPosition) const;

    // Total kekayaan pemain (uang + harga beli properti + bangunan)
    int calculateWealth(Player *player) const;

    // Log transaksi (shortcut agar Command tidak perlu akses logger langsung)
    void log(const std::string &username, const std::string &action, const std::string &detail);

    void useSkillCard(Player *player, SkillCard *card, GameState &gs);

    // Proxy metode dialog
    void handleGadai(Property *prop);
    void handleTebus(Property *prop);
    void handleBangun(StreetProperty *sp);
    void handleJualBangunan(StreetProperty *sp);

    void handleSkillCardOverflow(Player *player);
    void giveSkillCardToPlayer(Player *player, SkillCard *card);
};

#endif