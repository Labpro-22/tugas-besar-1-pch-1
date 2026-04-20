#include "TransactionLogger.hpp"
#include <algorithm>

void TransactionLogger::addLog(int turn, std::string name, std::string action, std::string detail){
    LogEntry entry;
    entry.turn = turn;
    entry.username = name;
    entry.actionType = action;
    entry.detail = detail;
    logs.push_back(entry);
}

const std::vector<LogEntry>& TransactionLogger::getLogs() const{
    return logs;
}

std::vector<LogEntry> TransactionLogger::getLastLogs(int n) const{
    if (logs.empty()) return {};
    
    int count = std::min((int)logs.size(), n);

    return std::vector<LogEntry>(logs.end() - count, logs.end());
}

void TransactionLogger::clear(){
    logs.clear();
}