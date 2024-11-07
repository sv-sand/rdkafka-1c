#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Strings.h"

class Loger
{
public:

    enum class Levels
    {
        NONE = 0,
        DEBUG,
        INFO,
        WARN,
        ERRORS
    };

    Levels level = Levels::NONE;

    Loger();
    ~Loger();
    
    bool Init(std::string FileName);
    bool Init(std::string FileName, std::string &ErrorDescription);
    
    void Debug(std::string Message);
    void Info(std::string Message);
    void Warn(std::string Message);
    void Error(std::string Message);
    
    std::string GetLogFile();

private:
    std::string fileName;

    bool ClearFile(std::string FileName);
    void WriteFile(std::string FileName, std::string Message);
};

