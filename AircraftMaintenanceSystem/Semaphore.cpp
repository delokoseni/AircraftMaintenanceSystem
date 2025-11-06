#include <iostream>
#include <Windows.h>
#include <string.h>
#include "Semaphore.h"

Semaphore::Semaphore(const char* name, int state)
{
    int len = strlen(name) + 1;
    this->name = new char[len];
    memcpy(this->name, name, len);

    wchar_t wname[256];
    MultiByteToWideChar(CP_ACP, 0, this->name, -1, wname, 256);

    this->handle = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, wname);
    if (this->handle == NULL)
    {
        this->handle = CreateSemaphoreW(NULL, state, LONG_MAX, wname);
    }
}


Semaphore::~Semaphore() {}

void Semaphore::P()
{
	WaitForSingleObject(handle, INFINITE);
}


void Semaphore::V()
{
	ReleaseSemaphore(handle, 1, NULL);
}