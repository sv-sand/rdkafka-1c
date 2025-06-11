#pragma once

#include <AddInNative.h>
#include "MockRdKafka1C.h"

namespace RdKafka1C {

    class MockAddInNative :
        public CAddInNative
    {
    public:
        bool ADDIN_API Init(void*) override;

        MockAddInNative() :
            CAddInNative() {
        };

        MockRdKafka1C* GetRdk1C();
    };

}