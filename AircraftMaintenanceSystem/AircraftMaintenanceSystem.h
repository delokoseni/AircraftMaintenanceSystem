#pragma once
#include <string>
#include <iomanip>
#include "Semaphore.h"

void Log(const char* component, const std::string& text, int id = -1);
extern Semaphore printSem;
