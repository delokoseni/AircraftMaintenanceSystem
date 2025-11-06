#include <iostream>
#include <Windows.h>
#include <string.h>
#include "Semaphore.h"

Semaphore::Semaphore(const char* name, int state)
{
	int len = strlen(name) + 1;
	this->name = new char(len);
	memcpy(this->name, name, len);

	this->handle = OpenSemaphore(SEMAPHORE_ALL_ACCESS, true, (LPCWSTR)this->name);
	if (this->handle == NULL)
	{
		this->handle = CreateSemaphore(NULL, state, 1, (LPCWSTR)this->name);
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