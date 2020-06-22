#include "BMPFILE.h"
#include <algorithm>

constexpr auto M_PI = 3.141592653589793238462643383279502884L;

static class BMPHelper
{
public:
	static BMPFILE ReadFromFile(std::string path);
	static void BlurByWidth(BMPFILE* originalBmp, BMPFILE* bluredBmp, int startWidth, int endWidth, int radius = 5);
	static void WriteBMPFile(BMPFILE* bmp, std::string path);

	BMPHelper(){};
	~BMPHelper(){};
};