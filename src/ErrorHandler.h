#pragma once

#include <string>
#include "Loger.h"

class ErrorHandler {
    
    public:
        ErrorHandler(Loger* Loger);

        bool Error();
        std::string ErrorDescription();
        void Set(std::string ErrorDescription);
        void Clear();

    private:
        Loger* loger;
        bool error;
        std::string errorDescription;
};