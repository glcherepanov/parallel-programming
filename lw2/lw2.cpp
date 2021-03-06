#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <time.h>
#include "BMPFile.h"
#include "BMPHelper.h"

struct Params
{
	BMPFILE* bmp;
	BMPFILE* blured;
	int start;
	int end;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	struct Params* params = (struct Params*)lpParam;
	BMPHelper::BlurByWidth(&*params->bmp, &*params->blured, params->start, params->end);
	ExitThread(0);
}

int main(int argc, char* argv[])
{
	clock_t start = clock();
	if (argc != 5)
	{
		return -1;
	}
	std::string bmpFileName = argv[1];
	std::string bluredFileName = argv[2];

	BMPFILE bmp = BMPHelper::ReadFromFile(bmpFileName);

	if (bmp.GetPixels().size() == 0)
	{
		return -1;
	}

	BMPFILE blured = BMPFILE(bmp);

	int threadsCount = std::stoi(argv[3]);
	int coreCount = std::stoi(argv[4]);
	uint32_t width = (bmp.GetWidth() - 1) / threadsCount;
	std::vector<Params> paramsToThread;

	int minWidth = 0;

	for (int i = 0; i < threadsCount; i++)
	{
		Params params;
		params.bmp = &bmp;
		params.blured = &blured;
		params.start = minWidth;
		params.end = i == threadsCount - 1 ? bmp.GetWidth() - 1 : minWidth + width;
		paramsToThread.push_back(params);
		minWidth += width;
	}

	HANDLE* handles = new HANDLE[threadsCount];

	for (size_t i = 0; i < threadsCount; i++)
	{
		handles[i] = CreateThread(NULL, i, &ThreadProc, &paramsToThread[i], CREATE_SUSPENDED, NULL);
		SetThreadAffinityMask(handles[i], 1 << i % coreCount);
	}

	for (int i = 0; i < threadsCount; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

	BMPHelper::WriteBMPFile(&blured, bluredFileName);

	std::cout << clock() - start << std::endl;
}
