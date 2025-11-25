#pragma once
#include <iostream>
#include <string.h>
#include <windows.h>
#include "Semaphore.h"

/*
Класс для реализации Канала, через обычные семафоры
*/
class Channel
{
private:
	Semaphore emptySem;
	Semaphore freeSem;
	HANDLE fileMem;		//файл для передачи данных
	void* buffer;			//переменная для хранения и передачи данных

public:
	Channel(const char* name);
	int get();					//получить данные
	void put(int);				// положить данные
	Channel();
	~Channel();
};

