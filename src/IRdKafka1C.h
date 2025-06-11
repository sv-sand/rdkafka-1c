#pragma once

#include <librdkafka/rdkafkacpp.h>
#include "Loger.h"
#include "ErrorHandler.h"

namespace RdKafka1C {

    class IRdKafka1C {
        public:

            int OperationTimeout = 1000; // ms

            virtual ~IRdKafka1C() = default;

            // Set RdKafka config property https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md
            // Invoke this method before InitProducer(), InitConsumer()
            virtual void SetConfigProperty(std::string Name, std::string Value) = 0;
            virtual std::string RdKafkaVersion() = 0;
        
            // Producer
            virtual bool InitProducer() = 0;
            virtual bool StopProduser() = 0;
            virtual bool StartProduce() = 0;
            virtual bool Produce(std::string Topic, std::string Message, std::string Key, std::string Headers, int Partition, std::string MessageId) = 0;
            virtual bool Flush() = 0;
            virtual int ProducerQueueLen() = 0;
            virtual int CountUndeliveredMessages() = 0;
            virtual std::string MessageStatus(std::string MessageId) = 0;

            // Consumer
            virtual bool InitConsumer() = 0;
            virtual bool StopConsumer() = 0;
            virtual bool Consume() = 0;
            virtual std::string MessageData() = 0;
            virtual std::string MessageKey() = 0;
            virtual std::string MessageHeaders() = 0;
            virtual std::string MessageMetadata() = 0;
            virtual int64_t CommittedOffset(std::string Topic, int Partition) = 0;
            virtual bool AssignPartition(std::string Topic, int Partition) = 0;
            virtual bool CommitOffset() = 0;
            virtual bool ChangeOffset(std::string Topic, int Partition, int64_t Offset) = 0;
            virtual int ConsumerQueueLen() = 0;

            // Subscriptions
            virtual std::string Subscription() = 0;
            virtual bool Subscribe(std::string Topic) = 0;
            virtual bool Unsubscribe() = 0;

            // Other
            virtual std::string MessageStatusToString(RdKafka::Message::Status Status) = 0;
        
    };

}