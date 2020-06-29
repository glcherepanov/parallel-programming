#include "LogBuffer.h"

LogBuffer::LogBuffer(LogFileWriter* logFileWriter)
	:_writer(logFileWriter)
	,_cs(CRITICAL_SECTION())
{
	InitializeCriticalSectionAndSpinCount(&_cs, 0x00000400);

	_eventHandle = CreateEvent(nullptr, true, false, L"Event");
	CreateWritingThread();
}

LogBuffer::~LogBuffer()
{
	EnterCriticalSection(&_cs);
	SetEvent(_eventHandle);

	WaitForSingleObject(_threadHandle, INFINITE);
	LeaveCriticalSection(&_cs);

	DeleteCriticalSection(&_cs);
	CloseHandle(_eventHandle);
	CloseHandle(_threadHandle);

}

void LogBuffer::Add(std::string value)
{
	EnterCriticalSection(&_cs);

	if (_list.Count() == BUFFER_SIZE)
	{
		SetEvent(_eventHandle);

		if (WaitForSingleObject(_threadHandle, INFINITE) == WAIT_OBJECT_0)
		{
			CreateWritingThread();
		}
	}

	_list.Add(value);

	LeaveCriticalSection(&_cs);
}

void LogBuffer::CreateWritingThread()
{
	ResetEvent(_eventHandle);
	_threadHandle = CreateThread(NULL, 0, &LogSizeMonitiringThread, (void*)this, 0, NULL);
}

DWORD WINAPI LogBuffer::LogSizeMonitiringThread(CONST LPVOID lpParam)
{
	LogBuffer* params = (LogBuffer*)lpParam;

	if (WaitForSingleObject(params->_eventHandle, INFINITE) == WAIT_OBJECT_0)
	{
		while (params->_list.Count() != 0)
		{
			params->_writer->Write(params->_list.Pop());
		}
	}

	ExitThread(0);
}