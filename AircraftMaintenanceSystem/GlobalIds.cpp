#include "GlobalIds.h"
#include <Windows.h>
#include <iostream>

struct GlobalCounters {
    unsigned int airplaneCounter;
};

static HANDLE gMem = NULL;
static GlobalCounters* gCounters = NULL;
static HANDLE gMutex = NULL;

static void InitGlobal()
{
    if (gCounters != nullptr) return;

    wchar_t memName[] = L"GlobalCountersMem";
    wchar_t mutexName[] = L"GlobalCountersMutex";

    gMem = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, memName);
    if (!gMem) {
        gMem = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(GlobalCounters), memName);
    }
    gCounters = (GlobalCounters*)MapViewOfFile(gMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GlobalCounters));

    gMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, mutexName);
    if (!gMutex) {
        gMutex = CreateMutexW(NULL, FALSE, mutexName);
    }

    // Инициализация только при первом создании
    if (gCounters->airplaneCounter == 0) {
        gCounters->airplaneCounter = 0;
    }
}

static unsigned int Next(unsigned int GlobalCounters::* field)
{
    InitGlobal();
    WaitForSingleObject(gMutex, INFINITE);
    unsigned int value = ++(gCounters->*field);
    ReleaseMutex(gMutex);
    return value;
}

unsigned int GetNextAirplaneId() { return Next(&GlobalCounters::airplaneCounter); }

