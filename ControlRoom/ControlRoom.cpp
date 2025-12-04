#include <iostream>
#include <iomanip>
#include <ctime>
#include <Windows.h>
#include "AircraftMaintenanceSystem.h"
#include "RunwayRW.h"


bool systemRunning = true;


int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Log("Диспетчерская", "Запущена");


	// Семафоры для автотрапа и погрузчика оставлены как раньше
	Semaphore autotrapSem("AutotrapSem", 0);
	Semaphore loaderSem("LoaderSem", 0);


	while (systemRunning) {
		RunwayState rs;
		if (!ReadRunwayState(rs)) {
			Log("Диспетчерская", "Не удалось прочитать состояние ВПП");
			Sleep(1000);
			continue;
		}


		// Логика принятия решения
		Log("Диспетчерская", std::string("Проверка ВПП: surface=" + std::to_string(rs.surfaceCondition) + ", vis=" + std::to_string(rs.visibility)));


		if (rs.surfaceCondition == 1 && rs.visibility > 30) {
			Log("Диспетчерская", "Посадка разрешена");
			// Ждём сообщение о приземлении из канала в старом варианте — оставляем упрощенно
			// Симуляция: если пришёл приземление — запускаем автотрап и loader
			// Для демонстрации — сразу запускаем сервисы
			Log("Диспетчерская", "Самолет приземлился, запускаю автотрап");
			autotrapSem.V();
			autotrapSem.P();
			Log("Диспетчерская", "Автотрап завершил, запускаю погрузчик");
			loaderSem.V();
			loaderSem.P();
		}
		else {
			Log("Диспетчерская", "Посадка запрещена");
		}


		Sleep(1000);
	}


	Log("Диспетчерская", "Конец работы.");
	return 0;
}