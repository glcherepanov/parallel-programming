#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <chrono>
#include "BMPFILE.h"
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

	int mode = std::stoi(argv[1]);
	int blocksCount = std::stoi(argv[2]);
	std::string inPath = argv[3];
	std::string outPath = argv[4];
	int threadCount = std::stoi(argv[5]);

	std::experimental::filesystem::create_directory(outPath);
	int fileNumber = 0;
	std::vector<Params> paramsToThread;

	for (auto& p : std::experimental::filesystem::directory_iterator(inPath))
	{
		BMPFILE* bmp = new BMPFILE(BMPHelper::ReadFromFile(p.path().string()));
		BMPFILE* blured = new BMPFILE(*bmp);

		uint32_t width = (bmp->GetWidth() - 1) / blocksCount;
		int minWidth = 0;
		for (int i = 0; i < blocksCount; i++)
		{
			Params params;
			params.bmp = bmp;
			params.blured = blured;
			params.start = minWidth;
			params.end = i == blocksCount - 1 ? bmp->GetWidth() - 1 : minWidth + width;
			paramsToThread.push_back(params);
			minWidth += width;
		}
		fileNumber++;
		HANDLE* handles = new HANDLE[paramsToThread.size()];
		if (mode == 0)
		{
			for (size_t i = 0; i < paramsToThread.size(); i++)
			{
				handles[i] = CreateThread(NULL, i, &ThreadProc, &paramsToThread[i], 0, NULL);
			}

			WaitForMultipleObjects(threadCount, handles, true, INFINITE);
		}
		else
		{
			for (size_t i = 0; i < paramsToThread.size(); i++)
			{
				handles[i] = CreateThread(NULL, i, &ThreadProc, &paramsToThread[i], CREATE_SUSPENDED, NULL);
			}

			int currentThreadCount = 0;
 			for (int i = 0; i < paramsToThread.size(); i++)
			{
				ResumeThread(handles[i]);
				currentThreadCount++;
				if (currentThreadCount == threadCount)
				{
					WaitForMultipleObjects(i + 1, handles, true, INFINITE);
				}
			}

			WaitForMultipleObjects(threadCount, handles, true, INFINITE);
		}

		BMPHelper::WriteBMPFile(blured, outPath + "/blured_" + p.path().stem().string() + ".bmp");
	}

	std::cout << clock() - start << std::endl;
}
