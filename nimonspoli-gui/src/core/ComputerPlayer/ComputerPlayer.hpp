#ifndef COMPUTER_PLAYER_HPP
#define COMPUTER_PLAYER_HPP

#include <string>
#include <vector>
#include "../Player/Player.hpp"
#include "../GameState/GameState.hpp"
#include "../GameMaster/GameMaster.hpp"

// ═══════════════════════════════════════════════════════════════════
//  Difficulty Level
//  Menentukan seberapa cerdas keputusan yang diambil COM
// ═══════════════════════════════════════════════════════════════════
enum class COMDifficulty {
    EASY,    // Keputusan acak / naif
    MEDIUM,  // Keputusan berbasis aturan sederhana
    HARD     // Keputusan strategis (heuristic-based)
};

std::string difficultyToString(COMDifficulty diff);

// ═══════════════════════════════════════════════════════════════════
//  ComputerPlayer
//  Merupakan subclass Player yang dapat mengambil keputusan sendiri.
//  Setiap aksi (beli, lelang, gadai, bangun, dll.) di-override
//  dengan logika yang disesuaikan difficulty.
// ═══════════════════════════════════════════════════════════════════
class ComputerPlayer : public Player {
private:
    COMDifficulty difficulty;

    // ── Utilitas internal ────────────────────────────────────────────
    // Hitung total biaya sewa maksimum yang bisa dikenakan properti ini
    int estimateRentValue(Property* prop, GameState& gs) const;

    // Hitung berapa properti dengan warna yang sama sudah dimiliki COM
    int countSameColorOwned(const std::string& colorGroup, GameState& gs) const;

    // Hitung total properti dalam satu color group di papan
    int countSameColorTotal(const std::string& colorGroup, GameState& gs) const;

    // Hitung skor strategis pembelian properti (HARD)
    double evaluatePropertyScore(Property* prop, GameState& gs) const;

    // Pilih properti terbaik untuk festival (HARD)
    Property* chooseBestFestivalProperty(GameState& gs) const;

    // Pilih properti terbaik untuk festival (MEDIUM)
    Property* chooseAnyFestivalProperty(GameState& gs) const;

    // Pilih properti yang paling menguntungkan untuk digadaikan
    Property* chooseMortgageTarget(int needed, GameState& gs) const;

    // Pilih properti dengan nilai sewa tinggi untuk dibangun
    Property* chooseBuildTarget(GameState& gs) const;

    // Utilitas: random bool dengan probabilitas tertentu (0.0–1.0)
    bool randomChance(double probability) const;

    // Log aksi COM ke layar agar pemain manusia tahu apa yang terjadi
    void announceAction(const std::string& action) const;

public:
    // ── Konstruktor ──────────────────────────────────────────────────
    ComputerPlayer(const std::string& username,
                   int startingBalance,
                   COMDifficulty difficulty);
    ~ComputerPlayer() override;

    // ── Getter ───────────────────────────────────────────────────────
    COMDifficulty getDifficulty() const;
    bool          isComputer()    const { return true; }

    // ── Aksi utama (dipanggil GameMaster / CommandDispatcher) ────────

    // Memutuskan apakah COM akan membeli properti street.
    // Mengembalikan true = beli, false = tolak → lelang.
    bool decideAndBuy(Property* prop, GameState& gs);

    // Memutuskan penawaran lelang.
    // Mengembalikan jumlah bid; 0 = PASS.
    int decideBid(Property* prop, int currentBid, GameState& gs);

    // Memutuskan properti mana yang digadaikan untuk mendapatkan uang.
    // Dipanggil saat COM butuh likuidasi aset.
    // Mengembalikan properti yang ingin digadaikan, atau nullptr jika tidak ada.
    Property* decideMortgage(int amountNeeded, GameState& gs);

    // Memutuskan apakah COM akan menebus properti yang tergadai.
    bool decideRedeem(Property* prop, GameState& gs);

    // Memutuskan properti mana yang akan dibangun rumah/hotel.
    // Mengembalikan pasangan <colorGroup, kode properti> atau {"", ""} jika tidak ada.
    std::pair<std::string, std::string> decideBuild(GameState& gs);

    // Memutuskan properti mana yang dipilih saat mendarat di Festival.
    // Mengembalikan kode properti yang dipilih, atau "" jika tidak ada.
    std::string decideFestivalProperty(GameState& gs);

    // Memutuskan apakah COM membayar denda penjara atau coba double dadu.
    // Mengembalikan true = bayar denda, false = coba lempar double.
    bool decidePayJailFine(GameState& gs);

    // Memutuskan pilihan pajak PPH: true = flat, false = persentase.
    bool decideIncomeTaxChoice(GameState& gs);

    // Memutuskan kartu Skill mana yang akan digunakan sebelum lempar dadu.
    // Mengembalikan indeks kartu di tangan (0-based), atau -1 jika tidak mau pakai.
    int decideUseSkillCard(GameState& gs);

    // Memutuskan kartu mana yang dibuang saat tangan penuh (4 kartu).
    // Mengembalikan indeks kartu yang akan dibuang (0-based).
    int decideDropCard(const std::vector<SkillCard*>& hand, SkillCard* newCard) const;

    // ── Eksekusi giliran penuh (dipanggil main loop) ─────────────────
    // Menjalankan seluruh giliran COM secara otomatis:
    //   1) Cek & gunakan skill card
    //   2) Lempar dadu (atau bayar penjara)
    //   3) Resolusi petak (sudah ditangani oleh onLanded)
    //   4) Opsional: bangun / tebus
    void executeTurn(GameMaster& gm);
};

#endif // COMPUTER_PLAYER_HPP