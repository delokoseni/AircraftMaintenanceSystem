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
    Semaphore loaderSem("LoaderSem", 0);
    Log("Погрузчик", "Запущен");

    while (systemRunning) {
        loaderSem.P();
        if (!systemRunning) break;
        Log("Погрузчик", "Начинаю обслуживание...");
        Sleep(2000);
        Log("Погрузчик", "Обслуживание завершено.");
        loaderSem.V();
    }

    Log("Погрузчик", "Конец работы.");
    return 0;
}
