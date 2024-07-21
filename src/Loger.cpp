#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include "Loger.h"

#ifdef _WINDOWS
#include <process.h>
#endif

const std::string TimeStamp();
const std::string TimeStamp(std::string format);
void Write(std::string FileName, std::string Message);
std::string Replace(std::string str, const std::string& from, const std::string& to);
std::string AddFinalSlash(std::string Path);

/////////////////////////////////////////////////////////////////////////////
// Class members

Loger::Loger()
{

#ifdef _WINDOWS
    pid = _getpid();
#else
    pid = getpid();
#endif
    
}

Loger::~Loger()
{
    Info("Stop logging");
}

bool Loger::Init(std::string Directory)
{
    std::string ErrorDescription = "";
    return Init(Directory, ErrorDescription);
}

bool Loger::Init(std::string Directory, std::string &ErrorDescription)
{
    fileName = "";

    if (!std::filesystem::exists(Directory))
    {
        ErrorDescription = "Path '" + Directory + "' not found";
        return false;
    }
    
    std::stringstream stream;
    stream << AddFinalSlash(Directory) 
        << "RkKafka1C_" 
        << TimeStamp("%Y-%m-%d %H%M%S")
        << "_" << std::to_string(pid) 
        << ".log";
    
    fileName = stream.str();
    return true;
}

void Loger::Debug(std::string Message)
{
	if (level > Levels::DEBUG || fileName.empty())
		return;

    std::stringstream stream;
    stream << TimeStamp() << " [DEBUG] " << Message;

    Write(fileName, stream.str());

}

void Loger::Info(std::string Message)
{
    if (level > Levels::INFO || fileName.empty())
        return;

    std::stringstream stream;
    stream << TimeStamp() << " [INFO] " << Message;

    Write(fileName, stream.str());
}

void Loger::Warn(std::string Message)
{
    if (level > Levels::WARN || fileName.empty())
        return;

    std::stringstream stream;
    stream << TimeStamp() << " [WARNING] " << Message;

    Write(fileName, stream.str());
}

void Loger::Error(std::string Message)
{
    if (level > Levels::ERRORS || fileName.empty())
        return;

    std::stringstream stream;
    stream << TimeStamp() << " [ERROR] " << Message;

    Write(fileName, stream.str());
}

std::string Loger::GetLogFile()
{
    return fileName;
}

/////////////////////////////////////////////////////////////////////////////
// Supporting methods

void Write(std::string FileName, std::string Message)
{
    std::ofstream file(FileName, std::ios_base::app);
    if (file.is_open())
    {
        file << Message << std::endl;
    }
    file.close();
}

const std::string TimeStamp()
{
    return TimeStamp("%Y-%m-%d %T");
}

const std::string TimeStamp(std::string format)
{
    std::chrono::time_point now = std::chrono::high_resolution_clock::now();
    tm current{};

#ifdef _WINDOWS
    time_t time = std::time(NULL);
    localtime_s(&current, &time);
#else
    auto time = std::chrono::system_clock::to_time_t(now);
    current = *std::gmtime(&time);
#endif

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds> (now.time_since_epoch());
    auto nanosecond = duration.count() % 1000000000;

    std::ostringstream stream{};
    stream << std::put_time(&current, format.c_str()) << "." << nanosecond;

    return stream.str();
}

std::string Replace(std::string str, const std::string& from, const std::string& to) 
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}

std::string AddFinalSlash(std::string Path)
{
    if (Path.empty())
        return Path;
    
    if (Path.back() == '/' || Path.back() == '\\')
        return Path;

    if (Path.find('/'))
        Path += "/";
    else if (Path.find('\\'))
        Path += "\\";

    return Path;
}