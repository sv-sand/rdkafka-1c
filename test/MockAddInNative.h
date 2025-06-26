#pragma once

#include <AddInNative.h>
#include "MockRdKafka1C.h"
#include "Loger.h"
#include "ErrorHandler.h"

namespace Kafka1C
{
    class MockAddInNative : public CAddInNative
    {
    public:
        MockAddInNative();

        bool ADDIN_API Init(void*) override;

        MockRdKafka1C* GetRdk1C();
    };
}