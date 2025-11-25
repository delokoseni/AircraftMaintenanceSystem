#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include "Channel.h"
#include "Semaphore.h"
#include "AircraftMaintenanceSystem.h"
#include "GlobalIds.h"

bool systemRunning = true;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Log("Самолет", "Запущен");

    Channel toControlRoom("AirplaneToControlRoom");
    Channel toControlRoomLandedMessage("AirplaneToControlRoomLandedMessage");
    Channel fromControlRoom("ControlRoomToAirplane");

    while (systemRunning) {
        unsigned int airplaneID = GetNextAirplaneId();

        Log("Самолет", "Запрашиваю посадку", airplaneID);
        toControlRoom.put(airplaneID);
        int result = fromControlRoom.get();

        if (result == 0) {
            Log("Самолет", "Посадка запрещена, иду на второй круг", airplaneID);
        }
        else {
            Log("Самолет", "Посадка разрешена, иду на посадку", airplaneID);
            toControlRoomLandedMessage.put(0);
            airplaneID++;
        }

        Sleep(2000);
    }

    Log("Самолет", "Конец работы.");
    return 0;
}
