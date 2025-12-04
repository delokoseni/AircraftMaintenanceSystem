#include <iostream>
#include <Windows.h>
#include "Channel.h"
#include "Semaphore.h"
#include "AircraftMaintenanceSystem.h"
#include "RunwayRW.h"

bool systemRunning = true;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Log("Диспетчерская", "Запущена");

    Channel fromAirplane("AirplaneToControlRoom");
    Channel fromAirplaneLandedMessage("AirplaneToControlRoomLandedMessage");
    Channel toAirplane("ControlRoomToAirplane");

    Semaphore autotrapSem("AutotrapSem", 0);
    Semaphore loaderSem("LoaderSem", 0);

    // Инициализация читателя
    InitRunwayRW();

    while (systemRunning) {
        int airplaneID = fromAirplane.get();
        Log("Диспетчерская", "Приняла запрос на посадку", airplaneID);

        RunwayState st;
        ReadRunwayState(st);

        // Используем surfaceCondition
        if (st.surfaceCondition == 1) {
            Log("Диспетчерская", "ВПП в хорошем состоянии, посадка разрешена", airplaneID);
            toAirplane.put(1);

            int landed = fromAirplaneLandedMessage.get();
            if (landed == 0) {
                Log("Диспетчерская", "Самолет приземлился, запускаю автотрап", airplaneID);
                autotrapSem.V();
                autotrapSem.P();

                Log("Диспетчерская", "Запускаю погрузчик", airplaneID);
                loaderSem.V();
                loaderSem.P();
            }
        }
        else {
            Log("Диспетчерская", "ВПП в плохом состоянии, посадка запрещена", airplaneID);
            toAirplane.put(0);
        }

        Sleep(600);
    }

    CloseRunwayRW();
    Log("Диспетчерская", "Конец работы.");
    return 0;
}
