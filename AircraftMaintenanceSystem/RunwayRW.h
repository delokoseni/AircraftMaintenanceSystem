#pragma once
#include <Windows.h>
#include <string>

struct RunwayState {
    int surfaceCondition; // 1 - good, 0 - bad
    int visibility;       // 0..100
    float temperature;    // ambient temperature
    long timestamp;       // epoch time
};

// Инициализация RW
bool InitRunwayRW();
void CloseRunwayRW();

// Чтение/запись
bool ReadRunwayState(RunwayState& out);
bool WriteRunwayState(const RunwayState& st);
