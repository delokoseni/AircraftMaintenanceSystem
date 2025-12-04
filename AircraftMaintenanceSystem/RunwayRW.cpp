#include "RunwayRW.h"
#include <windows.h>
#include <iostream>

static HANDLE hMap = NULL;
static RunwayState* shared = NULL;

static HANDLE hMutexWrite = NULL;
static HANDLE hMutexRCount = NULL;
static HANDLE hSemReaders = NULL;

static int* readerCount = nullptr;

bool InitRunwayRW() {
    // Shared memory (RunwayState + readerCount)
    hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"RunwaySharedMem");
    if (!hMap)
        hMap = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
            sizeof(RunwayState) + sizeof(int),
            L"RunwaySharedMem");

    if (!hMap) return false;

    void* base = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0,
        sizeof(RunwayState) + sizeof(int));
    if (!base) return false;

    shared = reinterpret_cast<RunwayState*>(base);
    readerCount = reinterpret_cast<int*>((char*)base + sizeof(RunwayState));

    // Semaphores & mutexes
    hMutexWrite = CreateMutexW(NULL, FALSE, L"RunwayWriteMutex");
    hMutexRCount = CreateMutexW(NULL, FALSE, L"RunwayRCountMutex");
    hSemReaders = CreateSemaphoreW(NULL, 1, 1, L"RunwayReadersBlock");

    return true;
}

void CloseRunwayRW() {
    if (shared) UnmapViewOfFile(shared);
    if (hMap) CloseHandle(hMap);
    if (hMutexWrite) CloseHandle(hMutexWrite);
    if (hMutexRCount) CloseHandle(hMutexRCount);
    if (hSemReaders) CloseHandle(hSemReaders);
}

// ---------------------------------------------------------------------------
// READER — диспетчерская
// ---------------------------------------------------------------------------
bool ReadRunwayState(RunwayState& out) {
    WaitForSingleObject(hMutexRCount, INFINITE);

    (*readerCount)++;
    if (*readerCount == 1)
        WaitForSingleObject(hSemReaders, INFINITE);

    ReleaseMutex(hMutexRCount);

    // ---- ЧТЕНИЕ ----
    out = *shared;

    // ---- END READ ----
    WaitForSingleObject(hMutexRCount, INFINITE);

    (*readerCount)--;
    if (*readerCount == 0)
        ReleaseSemaphore(hSemReaders, 1, NULL);

    ReleaseMutex(hMutexRCount);

    return true;
}

// ---------------------------------------------------------------------------
// WRITER — датчики ВПП
// ---------------------------------------------------------------------------
bool WriteRunwayState(const RunwayState& st) {
    WaitForSingleObject(hMutexWrite, INFINITE);
    WaitForSingleObject(hSemReaders, INFINITE);

    *shared = st;

    ReleaseSemaphore(hSemReaders, 1, NULL);
    ReleaseMutex(hMutexWrite);
    return true;
}
