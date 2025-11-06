#include <iostream>
#include <sstream>
#include <random>
#include "Channel.h"

DWORD WINAPI AirplaneThread(PVOID p);
DWORD WINAPI ControlRoomThread(PVOID p);
DWORD WINAPI RunwaySensorThread(PVOID p);
DWORD WINAPI AutotrapThread(PVOID p);
DWORD WINAPI LoaderThread(PVOID p);

bool systemRunning = true;

Semaphore printSem("PrintSem", 1);

static void Log(const char* component, const std::string& text, int id = -1) {
    printSem.P();
    time_t t = time(NULL);
    struct tm tm;
    localtime_s(&tm, &t);
    char timebuf[16];
    sprintf_s(timebuf, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

    if (id >= 0) {
        std::cout << "[" << timebuf << "] [" << component << "] (ID:" << id << ") " << text << std::endl;
    }
    else {
        std::cout << "[" << timebuf << "] [" << component << "] " << text << std::endl;
    }

    printSem.V();
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::cout << "Система диспетчеризации и обслуживания прибытия пассажирских самолетов" << std::endl;
    std::cout << "Запуск подсистем..." << std::endl;

    HANDLE threads[5] = { NULL, NULL, NULL, NULL, NULL };

    threads[1] = CreateThread(NULL, 0, ControlRoomThread, NULL, 0, NULL);
    Sleep(300);
    threads[2] = CreateThread(NULL, 0, RunwaySensorThread, NULL, 0, NULL);
    Sleep(300);
    threads[3] = CreateThread(NULL, 0, AutotrapThread, NULL, 0, NULL);
    Sleep(300);
    threads[4] = CreateThread(NULL, 0, LoaderThread, NULL, 0, NULL);
    Sleep(300);
    threads[0] = CreateThread(NULL, 0, AirplaneThread, NULL, 0, NULL);

    for (int i = 0; i < 30 && systemRunning; i++) {
        Sleep(1000);
    }

    systemRunning = false;

    // Gracefully close thread handles
    for (int i = 0; i < 5; i++) {
        if (threads[i] && threads[i] != INVALID_HANDLE_VALUE) {
            DWORD waitResult = WaitForSingleObject(threads[i], 3000);
            if (waitResult == WAIT_TIMEOUT) {
                std::cout << "Поток " << i << " не завершился вовремя, выполняется принудительное завершение" << std::endl;
            }
            CloseHandle(threads[i]);
        }
    }
    std::cout << "Система завершила работу" << std::endl;
    return 0;
}

DWORD WINAPI AirplaneThread(PVOID p) {
    Log("Самолет", "Летит");
    Channel toControlRoom("AirplaneToControlRoom");
    Channel fromControlRoom("ControlRoomToAirplane");
    unsigned int airplaneID = 1;
    while (systemRunning) {
        {
            std::ostringstream ss;
            ss << "Запрашиваю посадку";
            Log("Самолет", ss.str(), airplaneID);
        }
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
        if (!systemRunning) break;
    }
    Log("Самолет", "Конец работы.");
    return 0;
}

DWORD WINAPI ControlRoomThread(PVOID p) {
    Log("Диспетчерская", "Запущена");
    Channel fromAirplane("AirplaneToControlRoom");
    Channel toAirplane("ControlRoomToAirplane");
    Channel fromRunwaySensor("RunwaySensorToControlRoom");
    Channel toRunwaySensor("ControlRoomToRunwaySensor");
    while (systemRunning) {
        int  airplaneID = fromAirplane.get();
        {
            std::ostringstream ss;
            ss << "Приняла запрос на посадку";
            Log("Диспетчерская", ss.str(), airplaneID);
        }
        Log("Диспетчерская", "Запрашивает состяние ВПП", airplaneID);
        toRunwaySensor.put(0);
        int result = fromRunwaySensor.get();
        {
            std::ostringstream ss;
            ss << "Состояние ВПП: " << result;
            Log("Диспетчерская", ss.str(), airplaneID);
        }
        if (result == 0) {
            {
                std::ostringstream ss;
                ss << "Неудовлетворительное состояние ВПП, посадка запрещена (" << result << ")";
                Log("Диспетчерская", ss.str(), airplaneID);
            }
            toAirplane.put(0);
            continue;
        }
        else {
            {
                std::ostringstream ss;
                ss << "Удовлетворительное состояние ВПП, посадка разрешена (" << result << ")";
                Log("Диспетчерская", ss.str(), airplaneID);
            }
            toAirplane.put(1);
            int isLanded = fromAirplane.get();
            if (isLanded == 0) {
                {
                    std::ostringstream ss;
                    ss << "Приземлился самолет, отправляю сигналы автотрапу и погрузчику.";
                    Log("Диспетчерская", ss.str(), isLanded);
                }

            }
        }
        if (!systemRunning) break;
    }
    Log("Диспетчерская", "Конец работы.");
    return 0;
}

DWORD WINAPI RunwaySensorThread(PVOID p) {
    Channel toControlRoom("RunwaySensorToControlRoom");
    Channel fromControlRoom("ControlRoomToRunwaySensor"); 
    Log("Датчики ВПП", "Запущены");

    while (systemRunning) {
        int request = fromControlRoom.get(); // ждём запроса от диспетчера
        if (!systemRunning) break;

        int randomValue = rand() % 2;
        toControlRoom.put(randomValue);

        {
            std::ostringstream ss;
            ss << "Отправлено состояние ВПП: " << randomValue;
            Log("Датчики ВПП", ss.str());
        }
    }

    Log("Датчики ВПП", "Конец работы.");
    return 0;
}


DWORD WINAPI AutotrapThread(PVOID p) {
    Log("Автотрап", "Запущен");
    while (systemRunning) {
        if (!systemRunning) break;
    }
    Log("Автотрап", "Конец работы.");
    return 0;
}

DWORD WINAPI LoaderThread(PVOID p) {
    Log("Погрузчик", "Запущен");
    while (systemRunning) {
        if (!systemRunning) break;
    }
    Log("Погрузчик", "Конец работы.");
    return 0;
}