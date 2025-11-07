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
    Log("Диспетчерская", "Запущена");

    Channel fromAirplane("AirplaneToControlRoom");
    Channel toAirplane("ControlRoomToAirplane");
    Channel fromRunwaySensor("RunwaySensorToControlRoom");
    Channel toRunwaySensor("ControlRoomToRunwaySensor");

    Semaphore autotrapSem("AutotrapSem", 0);
    Semaphore loaderSem("LoaderSem", 0);

    while (systemRunning) {
        int airplaneID = fromAirplane.get();
        Log("Диспетчерская", "Приняла запрос на посадку", airplaneID);

        Log("Диспетчерская", "Запрашивает состояние ВПП", airplaneID);
        toRunwaySensor.put(0);
        int result = fromRunwaySensor.get();

        if (result == 0) {
            Log("Диспетчерская", "Посадка запрещена", airplaneID);
            toAirplane.put(0);
        }
        else {
            Log("Диспетчерская", "Посадка разрешена", airplaneID);
            toAirplane.put(1);
            int landed = fromAirplane.get();
            if (landed == 0) {
                Log("Диспетчерская", "Самолет приземлился, запускаю автотрап", airplaneID);
                autotrapSem.V();
                autotrapSem.P();
                Log("Диспетчерская", "Автотрап завершил, запускаю погрузчик", airplaneID);
                loaderSem.V();
                loaderSem.P();
            }
        }

        Sleep(1000);
    }

    Log("Диспетчерская", "Конец работы.");
    return 0;
}
