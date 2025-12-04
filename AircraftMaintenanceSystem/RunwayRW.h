#pragma once
#include "RunwayState.h"
#include "Semaphore.h"
#include <Windows.h>


// Имена семафоров 
constexpr char RUNWAY_MUTEX_NAME[] = "Runway_mutex";
constexpr char RUNWAY_ROOMEMPTY_NAME[] = "Runway_roomEmpty";
constexpr char RUNWAY_BATON_NAME[] = "Runway_baton";


// API
bool InitRunwayRW();
void CloseRunwayRW();


// Читает snapshot состояния (не блокирует других читателей)
bool ReadRunwayState(RunwayState& outState);


// Пишет новое состояние
bool WriteRunwayState(const RunwayState& newState);