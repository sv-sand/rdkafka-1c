#include "Loger.h"

namespace Kafka1C {

    Loger::Loger() {
    }

    Loger::~Loger() {
        Info("Stop logging");
    }

    bool Loger::Init(std::string FileName) {
        std::string ErrorDescription = "";
        return Init(FileName, ErrorDescription);
    }

    bool Loger::Init(std::string FileName, std::string& ErrorDescription) {
        fileName = FileName;

        if (!StartNewFile(fileName)) {
            ErrorDescription = "Failed to open file " + fileName;
            return false;
        }

        return true;
    }

    void Loger::Debug(std::string Message) {
        if (level > Levels::DEBUG || level == Levels::NONE || fileName.empty())
            return;

        std::stringstream stream;
        stream << Strings::TimeStamp() << " [DEBUG] " << Message;

        WriteFile(fileName, stream.str());
    }

    void Loger::Info(std::string Message) {
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

    void Loger::Error(std::string Message) {
        if (level > Levels::ERRORS || level == Levels::NONE || fileName.empty())
            return;

        std::stringstream stream;
        stream << Strings::TimeStamp() << " [ERROR] " << Message;

        WriteFile(fileName, stream.str());
    }

    std::string Loger::GetLogFile() {
        return fileName;
    }

    /////////////////////////////////////////////////////////////////////////////
    // File operations

    bool Loger::StartNewFile(std::string FileName) {
        bool result;
        std::ofstream file(FileName, std::ios::out | std::ios::trunc | std::ios::binary);

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

    void Loger::WriteFile(std::string FileName, std::string Message) {
        std::ofstream file(FileName, std::ios::app);

        if (file.is_open())
            file << Message << std::endl;

        file.close();
    }

} // namespace RdKafka1C