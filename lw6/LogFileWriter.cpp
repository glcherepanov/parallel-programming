#include "LogFileWriter.h"

LogFileWriter::LogFileWriter(std::string fileName)
	:fout(std::ofstream(fileName))
{
}

void LogFileWriter::Write(std::string value)
{
	fout << value << std::endl;
}
