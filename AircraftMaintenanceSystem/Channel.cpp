#include "Channel.h"


Channel::Channel(const char* name) {

	int len = strlen(name) + 1;
	char* name1 = new char(len + 1);
	memcpy(name1 + 1, name, len);

	name1[0] = '1';
	freeSem = Semaphore(name1, 1);

	name1[0] = '2';
	emptySem = Semaphore(name1, 0);

	fileMem = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		false,
		(LPCWSTR)name1
	);
	buffer = NULL;
	_Post_equals_last_error_ DWORD error;
	if (fileMem == NULL)
		fileMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, (LPCWSTR)name1);
	if (fileMem != NULL)
		buffer = MapViewOfFile(fileMem, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
	else {
		error = GetLastError();
		std::cout << error << std::endl;
		printf("error: FILE_MAP \n");
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
