#include "Loger.h"

bool ClearFile(std::string FileName);
void WriteFile(std::string FileName, std::string Message);

/////////////////////////////////////////////////////////////////////////////
// Class members

Loger::Loger()
{   
}

Loger::~Loger()
{
    Info("Stop logging");
}

bool Loger::Init(std::string FileName)
{
    std::string ErrorDescription = "";
    return Init(FileName, ErrorDescription);
}

bool Loger::Init(std::string FileName, std::string &ErrorDescription)
{
    fileName = FileName;

    if (!ClearFile(fileName))
    {
        ErrorDescription = "Failed to open file " + fileName;
        return false;
    }

    return true;
}

void Loger::Debug(std::string Message)
{
	if (level > Levels::DEBUG || fileName.empty())
		return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [DEBUG] " << Message;

    WriteFile(fileName, stream.str());

}

void Loger::Info(std::string Message)
{
    if (level > Levels::INFO || fileName.empty())
        return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [INFO] " << Message;

    WriteFile(fileName, stream.str());
}

void Loger::Warn(std::string Message)
{
    if (level > Levels::WARN || fileName.empty())
        return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [WARNING] " << Message;

    WriteFile(fileName, stream.str());
}

void Loger::Error(std::string Message)
{
    if (level > Levels::ERRORS || fileName.empty())
        return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [ERROR] " << Message;

    WriteFile(fileName, stream.str());
}

std::string Loger::GetLogFile()
{
    return fileName;
}

/////////////////////////////////////////////////////////////////////////////
// File operations

bool ClearFile(std::string FileName)
{
    bool result;
    std::ofstream file(FileName, std::ios_base::app);

    result = file.is_open();
    if (result)
        file.clear();
   
    file.close();

    return result;
}

void WriteFile(std::string FileName, std::string Message)
{
    std::ofstream file(FileName, std::ios_base::app);
    if (file.is_open())
        file << Message << std::endl;
    
    file.close();
}
