#pragma once

#include <string.h>
#include "Loger.h"

namespace Kafka1C {

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

} // namespace RdKafka1C