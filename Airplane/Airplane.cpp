#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include "Channel.h"
#include "Semaphore.h"

bool systemRunning = true;
Semaphore printSem("PrintSem", 1);

static void Log(const char* component, const std::string& text, int id = -1) {
    printSem.P();
    auto t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);
    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] [" << component << "]";
    if (id >= 0) std::cout << " (ID:" << id << ")";
    std::cout << " " << text << '\n';
    printSem.V();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Log("Самолет", "Запущен");

    Channel toControlRoom("AirplaneToControlRoom");
    Channel fromControlRoom("ControlRoomToAirplane");

    unsigned int airplaneID = 1;

    while (systemRunning) {
        Log("Самолет", "Запрашиваю посадку", airplaneID);
        toControlRoom.put(airplaneID);
        int result = fromControlRoom.get();

        if (result == 0) {
            Log("Самолет", "Посадка запрещена, иду на второй круг", airplaneID);
        }
        else {
            Log("Самолет", "Посадка разрешена, иду на посадку", airplaneID);
            toControlRoom.put(0);
            airplaneID++;
        }

        Sleep(2000);
    }

    Log("Самолет", "Конец работы.");
    return 0;
}
