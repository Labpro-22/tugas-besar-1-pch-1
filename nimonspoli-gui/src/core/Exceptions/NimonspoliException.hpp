#ifndef NIMONSPOLI_EXCEPTION_HPP
#define NIMONSPOLI_EXCEPTION_HPP

#include <exception>
#include <string>

class NimonspoliException : public std::exception {
protected:
    std::string message;
public:
    NimonspoliException(const std::string& msg) : message(msg) {}
    
    // Override method dari std::exception
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif