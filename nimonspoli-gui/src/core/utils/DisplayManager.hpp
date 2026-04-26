#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP
#include <string>
#include <iostream>

// DisplayManager.hpp — versi minimal
class DisplayManager {
public:
    void print(const std::string& msg) { std::cout << msg << "\n"; }
};

#endif