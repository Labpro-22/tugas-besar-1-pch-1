#ifndef TRANSACTION_LOGGER_HPP
#define TRANSACTION_LOGGER_HPP

#include <string>
#include <vector>

// Struktur data untuk setiap baris kejadian
// Contoh:
// [Turn 14] Uname3 | SEWA     | Bayar M10000 ke Uname2 (JKT, festival aktif x2)
struct LogEntry {
    int turn;
    std::string username;
    std::string actionType; // misal: "BELI", "SEWA", "DANA_UMUM", "PAJAK"
    std::string detail;     // misal: "Membeli Bandung seharga 200"
};

class TransactionLogger {
private:
    std::vector<LogEntry> logs;

public:
    TransactionLogger() = default;

    // Menambah catatan baru ke dalam daftar
    void addLog(int turn, std::string name, std::string action, std::string detail);

    // Mengambil semua log (untuk keperluan SAVE ke file .txt)
    const std::vector<LogEntry>& getLogs() const;

    // Mengambil N log terakhir untuk ditampilkan di UI/Terminal
    std::vector<LogEntry> getLastLogs(int n) const;

    // Menghapus semua log, dipakai saat game baru
    void clear();
};

#endif