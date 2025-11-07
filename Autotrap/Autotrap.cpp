#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include "Semaphore.h"
#include "AircraftMaintenanceSystem.h"

bool systemRunning = true;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    Semaphore autotrapSem("AutotrapSem", 0);
    Log("Автотрап", "Запущен");

    while (systemRunning) {
        autotrapSem.P();
        if (!systemRunning) break;
        Log("Автотрап", "Начинаю обслуживание...");
        Sleep(2000);
        Log("Автотрап", "Обслуживание завершено.");
        autotrapSem.V();
    }

    Log("Автотрап", "Конец работы.");
    return 0;
}
