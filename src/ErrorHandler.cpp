#include "ErrorHandler.h"

ErrorHandler::ErrorHandler(Loger* Loger) {
    loger = Loger;
}

bool ErrorHandler::Error() {
    return error;
}

std::string ErrorHandler::ErrorDescription() {
    return errorDescription;
}

void ErrorHandler::Set(std::string ErrorDescription) {
    error = true;
    errorDescription = ErrorDescription;

    if (loger)
        loger->Error(errorDescription);
}

void ErrorHandler::Clear() {
    error = false;
    errorDescription = "";
}
