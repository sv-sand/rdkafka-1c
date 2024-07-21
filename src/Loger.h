#pragma once

#include <string>

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
    
    bool Init(std::string Directory);
    bool Init(std::string Directory, std::string &ErrorDescription);
    
    void Debug(std::string Message);
    void Info(std::string Message);
    void Warn(std::string Message);
    void Error(std::string Message);
    
    std::string GetLogFile();

private:
    unsigned pid;
    std::string fileName;
};

