#pragma once
#include <Windows.h>
#include <string>


struct RunwayState {
	int surfaceCondition; // 0 = bad, 1 = ok
	int visibility; // 0..100
	float temperature; // Celsius
	long timestamp; // time_t
};


// Имя объекта разделяемой памяти
constexpr wchar_t RUNWAY_SHARED_MEM_NAME[] = L"GlobalRunwayStateMem";