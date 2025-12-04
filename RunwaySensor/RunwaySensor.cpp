#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>
#include "AircraftMaintenanceSystem.h"
#include "RunwayRW.h"

bool systemRunning = true;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Log("Датчики ВПП", "Запущены");

    InitRunwayRW();
    srand((unsigned)time(NULL) ^ GetCurrentProcessId());

    while (systemRunning) {
        RunwayState st;
        st.surfaceCondition = (rand() % 100 < 75) ? 1 : 0;
        st.visibility = 50 + rand() % 50;
        st.temperature = -5 + (rand() % 1500) / 100.0f;
        st.timestamp = (long)time(NULL);

        WriteRunwayState(st);

        Log("Датчики ВПП",
            "Состояние обновлено: покрытие=" + std::to_string(st.surfaceCondition));

        Sleep(2000);
    }

    CloseRunwayRW();
    Log("Датчики ВПП", "Конец работы.");
    return 0;
}
