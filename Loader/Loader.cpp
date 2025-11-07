#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
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
