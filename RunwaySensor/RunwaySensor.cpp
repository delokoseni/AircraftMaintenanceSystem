// RunwaySensor.cpp
#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include <cstdlib>
#include "AircraftMaintenanceSystem.h" // содержит Log(...) и printSem
#include "RunwayRW.h"

static volatile bool systemRunning = true;

// Обработчик консольных событий (Ctrl+C) — аккуратная остановка
BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT || signal == CTRL_BREAK_EVENT) {
        systemRunning = false;
        return TRUE;
    }
    return FALSE;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Установим обработчик консоли, чтобы корректно завершать процесс
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    Log("Датчик ВПП", "Запущен");

    // Инициализируем RW-модуль (создаст/откроет разделяемую память и семафоры)
    if (!InitRunwayRW()) {
        Log("Датчик ВПП", "Ошибка инициализации RunwayRW");
        return 1;
    }

    // Инициализация генератора случайных чисел
    srand((unsigned int)time(nullptr) ^ GetCurrentProcessId());

    // Цикл работы датчика — пишет периодически snapshot состояния ВПП
    while (systemRunning) {
        RunwayState ns;
        // Простая модель: 80% — хорошее покрытие, 20% — плохое
        ns.surfaceCondition = ((rand() % 100) < 80) ? 1 : 0;
        ns.visibility = rand() % 101; // 0..100
        ns.temperature = (rand() % 3500) / 100.0f - 10.0f; // -10.00 .. +24.99
        ns.timestamp = (long)time(nullptr);

        if (!WriteRunwayState(ns)) {
            Log("Датчик ВПП", "Не удалось записать состояние");
        }
        else {
            // Логируем коротко — не перегружая вывод
            Log("Датчик ВПП", std::string("Записано: surface=") + std::to_string(ns.surfaceCondition)
                + ", vis=" + std::to_string(ns.visibility)
                + ", temp=" + std::to_string(ns.temperature));
        }

        // Случайная пауза: 1.5s .. 3.5s
        int pauseMs = 1500 + (rand() % 2000);
        int slept = 0;
        // Спим небольшими кусками, чтобы реагировать на завершение
        while (systemRunning && slept < pauseMs) {
            Sleep(200);
            slept += 200;
        }
    }

    Log("Датчик ВПП", "Завершение работы, высвобождение ресурсов");
    CloseRunwayRW();
    Log("Датчик ВПП", "Конец работы.");
    return 0;
}
