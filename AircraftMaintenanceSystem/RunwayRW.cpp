#include "RunwayRW.h"
#include <iostream>
#include <chrono>
#include <cstring>


// Структура в общей памяти; содержит state и счётчик читателей
struct SharedRunway {
	RunwayState state;
	int readCount;
};


static HANDLE gMem = NULL;
static SharedRunway* gShared = nullptr;

// Семафоры
static Semaphore gMutex(RUNWAY_MUTEX_NAME, 1); // защитa readCount
static Semaphore gRoomEmpty(RUNWAY_ROOMEMPTY_NAME, 1); // для писателей (1 = свободно)
static Semaphore gBaton(RUNWAY_BATON_NAME, 1); // эстафета (инициално 1)


bool InitRunwayRW() {
	if (gShared != nullptr) return true;


	// Создаём/открываем память
	gMem = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, RUNWAY_SHARED_MEM_NAME);
	if (gMem == NULL) {
		gMem = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedRunway), RUNWAY_SHARED_MEM_NAME);
		if (!gMem) {
			DWORD err = GetLastError();
			std::cerr << "RunwayRW: CreateFileMapping failed: " << err << std::endl;
			return false;
		}
	}


	gShared = (SharedRunway*)MapViewOfFile(gMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedRunway));
	if (!gShared) {
		std::cerr << "RunwayRW: MapViewOfFile failed" << std::endl;
		return false;
	}


	// Инициализация при первом создании (мы предполагаем что нули уже)
	// Но чтобы быть уверенными — если readCount содержит мусор, установим нули
	// (это безопасно, если мы только что создали память)
	// Признак того что память новая — попробуем проверить timestamp
	if (gShared->readCount < 0 || gShared->readCount > 1000000) {
		gShared->readCount = 0;
	}


	return true;
}

void CloseRunwayRW() {
	if (gShared) {
		UnmapViewOfFile(gShared);
		gShared = nullptr;
	}
	if (gMem) {
		CloseHandle(gMem);
		gMem = NULL;
	}
}


bool ReadRunwayState(RunwayState& outState) {
	if (!InitRunwayRW()) return false;


	// Этап входа читателя (читатели имеют приоритет)
	// Захватываем мьютекс для readCount
	gMutex.P();
	gShared->readCount++;
	if (gShared->readCount == 1) {
		// первый читатель блокирует писателей
		gRoomEmpty.P();
	}
	gMutex.V();


	// Снимаем snapshot
	memcpy(&outState, &gShared->state, sizeof(RunwayState));


	// Выход читателя
	gMutex.P();
	gShared->readCount--;
	if (gShared->readCount == 0) {
		// последний читатель освобождает писателей
		gRoomEmpty.V();
	}
	gMutex.V();


	// Передача эстафеты (простая форма): вручим baton
	// (для этого реализован именованный семафор gBaton; но здесь мы просто V() — если кто-то ждет, он проснется)
	gBaton.V();


	return true;
}

bool WriteRunwayState(const RunwayState& newState) {
	if (!InitRunwayRW()) return false;


	// Для писателя: ждём пока нет активных читателей (roomEmpty)
	// Также экономим ресурс: блокируем батон, чтобы писатели шли по порядку


	// Предполагаем: gRoomEmpty имеет значение 1, P() заблокирует доступ
	gRoomEmpty.P(); // теперь писатель эксклюзивно владеет доступом


	// Записываем
	memcpy(&gShared->state, &newState, sizeof(RunwayState));


	// Обновим timestamp, если нужно (поле newState уже содержит timestamp)


	// Освобождаем комнату для читателей/писателей
	gRoomEmpty.V();


	// Передача эстафеты
	gBaton.V();


	return true;
}
