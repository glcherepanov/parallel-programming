#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <time.h>
#include "BMPFILE.h"
#include "BMPHelper.h"

struct Params
{
	clock_t startTime;
	int threadNumber;
	std::ofstream* fout;
	BMPFILE* bmp;
	BMPFILE* blured;
	int start;
	int end;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	struct Params* params = (struct Params*)lpParam;
	BMPHelper::BlurByWidth(&*params->bmp, &*params->blured, params->start, params->end, &*params->fout, params->startTime, params->threadNumber);
	ExitThread(0);
}

void WriteHowTo()
{
	std::cout << "Example: "
		<< "lw4.exe" << " "
		<< "input.bmp" << " "
		<< "output.bmp" << " "
		<< "threadCount" << " "
		<< "coreCount" << " "
		<< "priorities" << std::endl
		<< "Priorities:" << std::endl
		<< "-1 - below_normal" << std::endl
		<< "0 - normal" << std::endl
		<< "1 - above__normal" << std::endl;

}

int main(int argc, char* argv[])
{
	clock_t start = clock();

	if (argc == 2 && strcmp(argv[1], "/?"))
	{
		WriteHowTo();
	}
	if (argc != 5)
	{
		return -1;
	}
	std::string bmpFileName = argv[1];
	std::string bluredFileName = argv[2];

	std::vector<std::ofstream> outFiles;

	BMPFILE bmp = BMPHelper::ReadFromFile(bmpFileName);

	if (bmp.GetPixels().size() == 0)
	{
		return -1;
	}

	BMPFILE blured = BMPFILE(bmp);

	int threadsCount = std::stoi(argv[4]);
	int coreCount = std::stoi(argv[5]);
	uint32_t width = (bmp.GetWidth() - 1) / threadsCount;
	std::vector<int> priority;
	for (int i = 0; i < threadsCount; i++)
	{
		priority.push_back(std::stoi(argv[6 + i]));
		outFiles.push_back(std::ofstream("output" + std::to_string(i) + ".txt"));
	}

	std::vector<Params> paramsToThread;

	int minWidth = 0;

	for (int i = 0; i < threadsCount; i++)
	{
		Params params;
		params.startTime = start;
		params.threadNumber = i;
		params.fout = &outFiles[i];
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
		SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
		SetThreadPriority(handles[i], priority[i]);
	}

	for (int i = 0; i < threadsCount; i++)
	{
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

	BMPHelper::WriteBMPFile(&blured, bluredFileName);
}
