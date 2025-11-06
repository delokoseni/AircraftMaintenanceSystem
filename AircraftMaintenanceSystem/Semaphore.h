#pragma once
#include <iostream>
#include <Windows.h>
#include <string.h>

/*
* Класс для реализации Семафора
*/
class Semaphore
{
private:
	char* name;		//имя семафора
	HANDLE handle;	// для хранения указателя на именованый объект

public:
	Semaphore() {};
	Semaphore(const char* name, int state);
	~Semaphore();
	void P();		//ожидание действия
	void V();		//выполнение действия
};

