#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include "AircraftMaintenanceSystem.h"
#include "RunwayRW.h"
#include "GlobalIds.h"


bool systemRunning = true;


int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Log("Самолет", "Запущен");


	while (systemRunning) {
		unsigned int airplaneID = GetNextAirplaneId();


		RunwayState rs;
		if (!ReadRunwayState(rs)) {
			Log("Самолет", "Не удалось прочитать состояние ВПП", airplaneID);
			Sleep(1000);
			continue;
		}


		Log("Самолет", std::string("Состояние ВПП: surface=" + std::to_string(rs.surfaceCondition) + ", vis=" + std::to_string(rs.visibility)), airplaneID);


		// Простая логика посадки: если покрытие ок и видимость > 30 — разрешаем
		if (rs.surfaceCondition == 1 && rs.visibility > 30) {
			Log("Самолет", "Посадка разрешена, иду на посадку", airplaneID);
			// в оригинале писалось в канал о посадке — теперь просто логируем
		}
		else {
			Log("Самолет", "Посадка запрещена, иду на второй круг", airplaneID);
		}


		Sleep(2000);
	}


	Log("Самолет", "Конец работы.");
	return 0;
}