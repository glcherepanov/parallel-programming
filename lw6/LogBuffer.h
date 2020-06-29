#pragma once
#include <string>
#define NOMINMAX
#include <windows.h>
#include "List.h"
#include "LogFileWriter.h"

const int BUFFER_SIZE = 700;

class LogBuffer
{
public:
	LogBuffer(LogFileWriter* writer);
	~LogBuffer();
	void Add(std::string value);
private:
	void CreateWritingThread();
	static DWORD WINAPI LogSizeMonitiringThread(CONST LPVOID lpParam);
	LogFileWriter* _writer;
	CRITICAL_SECTION _cs;
	List _list;
	HANDLE _eventHandle;
	HANDLE _threadHandle;
};