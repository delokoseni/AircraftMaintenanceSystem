#include "AircraftMaintenanceSystem.h"
#include <iostream>
#include <ctime>

Semaphore printSem("PrintSem", 1);

void Log(const char* component, const std::string& text, int id) {
    printSem.P();
    auto t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);
    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] [" << component << "]";
    if (id >= 0) std::cout << " (ID:" << id << ")";
    std::cout << " " << text << '\n';
    printSem.V();
}
