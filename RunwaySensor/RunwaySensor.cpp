#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include <string>
#include "Channel.h"
#include "Semaphore.h"

bool systemRunning = true;
Semaphore printSem("PrintSem", 1);

static void Log(const char* component, const std::string& text) {
    printSem.P();
    auto t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);
    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] [" << component << "] " << text << '\n';
    printSem.V();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Log("Датчики ВПП", "Запущены");

    Channel toControlRoom("RunwaySensorToControlRoom");
    Channel fromControlRoom("ControlRoomToRunwaySensor");

    while (systemRunning) {
        int request = fromControlRoom.get();
        int randomValue = rand() % 2;
        toControlRoom.put(randomValue);
        Log("Датчики ВПП", "Отправлено состояние: " + std::to_string(randomValue));
        Sleep(1000);
    }

    Log("Датчики ВПП", "Конец работы.");
    return 0;
}
