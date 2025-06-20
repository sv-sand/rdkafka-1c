#pragma once

#include <RdKafka1C.h>
#include <gmock/gmock.h>

namespace RdKafka1C {

    class MockRdKafka1C :
        public RdKafka1C {
    public:
        MockRdKafka1C(Loger* Loger, ErrorHandler* Error) : 
            RdKafka1C(Loger, Error) {
            OperationTimeout = 1000;
        };

        MOCK_METHOD(RdKafka::ErrorCode, ProduserFlush, (), (override));
        MOCK_METHOD(RdKafka::Message*, ConsumerConsume, (), (override));
    };

}