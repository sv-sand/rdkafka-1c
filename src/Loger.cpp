#include "Loger.h"

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
    try
    {
        std::locale locale(DEFAULT_LOCALE);
    } catch(std::runtime_error e)
    {
        ErrorDescription = "OS doesn't support locale " + DEFAULT_LOCALE;
        return false;
    }
    
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
	if (level > Levels::DEBUG || level == Levels::NONE || fileName.empty())
		return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [DEBUG] " << Message;

    WriteFile(fileName, stream.str());

}

void Loger::Info(std::string Message)
{
    if (level > Levels::INFO || level == Levels::NONE || fileName.empty())
        return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [INFO] " << Message;

    WriteFile(fileName, stream.str());
}

void Loger::Warn(std::string Message)
{
    if (level > Levels::WARN || level == Levels::NONE || fileName.empty())
        return;

    std::stringstream stream;
    stream << Strings::TimeStamp() << " [WARNING] " << Message;

    WriteFile(fileName, stream.str());
}

void Loger::Error(std::string Message)
{
    if (level > Levels::ERRORS || level == Levels::NONE || fileName.empty())
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

bool Loger::ClearFile(std::string FileName)
{
    bool result;
    std::ofstream file(FileName, std::ios::out | std::ios::trunc);

    result = file.is_open();
    if (result)
        file 
            << "####--####---#--#--####--###--#--#--####----------#--####" << std::endl
            << "#--#--#--##--#-#---#--#--#----#-#---#--#---------##--#---" << std::endl
            << "####--#--##--##----####--###--##----####---###----#--#---" << std::endl
            << "#-#---#--##--#-#---#--#--#----#-#---#--#----------#--#---" << std::endl
            << "#-#---####---#--#--#--#--#----#--#--#--#----------#--####" << std::endl
            << std::endl;

    file.close();

    return result;
}

void Loger::WriteFile(std::string FileName, std::string Message)
{
    std::ofstream file(FileName, std::ios::app);
    std::locale locale(DEFAULT_LOCALE);
    file.imbue(locale);

    if (file.is_open())
        file << Message << std::endl;
    
    file.close();
}
