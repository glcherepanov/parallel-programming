#include "BMPFILE.h"

BMPFILE::BMPFILE(BMPINFO header, std::vector<std::vector<rgb>> pixels)
	:_bmpInfo(header)
	, _pixels(pixels)
{
}

BMPFILE::BMPINFO BMPFILE::GetInfo()
{
	return _bmpInfo;
}

std::vector<std::vector<BMPFILE::rgb>> BMPFILE::GetPixels()
{
	return _pixels;
}


uint32_t BMPFILE::GetWidth()
{
	return _bmpInfo.bi.biWidth;
}

uint32_t BMPFILE::GetHeight()
{
	return _bmpInfo.bi.biHeight;
}

BMPFILE::rgb BMPFILE::GetPixel(int x, int y)
{
	try
	{
		return _pixels[y][x];
	}
	catch (_exception)
	{
		std::cout << x << " " << y << std::endl;
	}
}

void BMPFILE::SetPixel(int x, int y, rgb pixel)
{
	try
	{
		_pixels[y][x] = pixel;
	}
	catch (_exception)
	{
		std::cout << x << " " << y << std::endl;
	}
}
