#ifndef TRANSACTION_LOGGER_HPP
#define TRANSACTION_LOGGER_HPP

#include <string>
#include <vector>

class LogEntry {
public:
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

    void addLog(int turn, std::string name, std::string action, std::string detail);
    const std::vector<LogEntry>& getLogs() const;
    std::vector<LogEntry> getLastLogs(int n) const;
    void clear();
};

#endif