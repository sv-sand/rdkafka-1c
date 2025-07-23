#pragma once

#include <RdKafka1C.h>
#include "MockProducer.h"
#include "MockKafkaConsumer.h"
#include <gmock/gmock.h>

namespace Kafka1C 
{

    class MockRdKafka1C : public RdKafka1C 
    {

    public:
        MockRdKafka1C(Loger* Loger, ErrorHandler* Error);

        bool InitProducer();
        bool InitConsumer();
        MockProducer* GetProducer();
        MockKafkaConsumer* GetConsumer();
    };

}