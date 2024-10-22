#include "time.hpp"
#include <chrono>

double GetCurrentTimeSeconds()
{
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>(now - start).count();
} 
