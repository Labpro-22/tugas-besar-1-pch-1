#include "TransactionLogger.hpp"
#include <algorithm>


const std::vector<LogEntry>& TransactionLogger::getLogs() const{
    return logs;
}

void TransactionLogger::setOnNewLog(std::function<void(const LogEntry&)> cb) {
    onNewLog = cb;
}

void TransactionLogger::addLog(int turn, std::string name, std::string action, std::string detail) {
    LogEntry e{turn, name, action, detail};
    logs.push_back(e);
    if (onNewLog) onNewLog(e);  // notify GUI
}


std::vector<LogEntry> TransactionLogger::getLastLogs(int n) const{
    if (logs.empty()) return {};
    
    int count = std::min((int)logs.size(), n);

    return std::vector<LogEntry>(logs.end() - count, logs.end());
}

void TransactionLogger::clear(){
    logs.clear();
}