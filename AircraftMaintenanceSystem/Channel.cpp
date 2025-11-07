#include "Channel.h"


Channel::Channel(const char* name) {
    int len = strlen(name) + 1;

    // Формируем корректные имена для семафоров
    char semFreeName[256];
    char semEmptyName[256];

    sprintf_s(semFreeName, "1_%s", name);
    sprintf_s(semEmptyName, "2_%s", name);

    freeSem = Semaphore(semFreeName, 1);
    emptySem = Semaphore(semEmptyName, 0);

    wchar_t wname[256];
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 256);

    fileMem = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, wname);
    if (fileMem == NULL)
        fileMem = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, wname);

    if (fileMem != NULL)
        buffer = MapViewOfFile(fileMem, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
    else {
        DWORD error = GetLastError();
        std::cout << error << std::endl;
        printf("error: FILE_MAP\n");
        buffer = NULL;
    }
}

//метод для того, чтобы положить в канал данные
void Channel::put(int data) {
	freeSem.P();
	*((int*)buffer) = data;
	emptySem.V();
}

//метод для того чтобы получить из канала данные
int  Channel::get() {
	int data;
	emptySem.P();
	data = *((int*)buffer);
	freeSem.V();
	return data;
}

Channel::Channel() {}

Channel ::~Channel() {}
