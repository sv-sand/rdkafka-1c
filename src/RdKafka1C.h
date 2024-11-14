#pragma once

#ifdef WIN32
#pragma comment(lib, "bcrypt.lib")
#endif

#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "Loger.h"
#include "ErrorHandler.h"
#include "ConfigBuilder.h"
#include "MessageStatusCollector.h"
#include "strings.h"
#include "utils.h"

class RdKafka1C
{
public:

    int OperationTimeout = 10000; // ms

    RdKafka1C(Loger* Loger, ErrorHandler* Error);
    ~RdKafka1C();

    std::string RdKafkaVersion();
    
    // Set RdKafka config property https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md
    // Invoke this method before InitProducer(), InitConsumer()
    void SetConfigProperty(std::string Name, std::string Value);

    // Producer
    bool InitProducer(std::string Brokers);
    bool StopProduser();
    bool Produce(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition);
    bool StartProduceAsynch();
    bool ProduceAsynch(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition);
    bool Flush();
    int ProducerQueueLen();

    // Consumer
    bool InitConsumer(std::string Brokers, std::string GroupId);
    bool StopConsumer();
    bool Consume();
    std::string GetMessageData();
    std::string GetMessageMetadata();
    int64_t CommittedOffset(std::string Topic, int Partition);
    bool AssignPartition(std::string Topic, int Partition);
    bool CommitOffset(std::string Topic, int Partition, int64_t Offset);
    int ConsumerQueueLen();

    // Subscriptions
    std::string Subscription();
    bool Subscribe(std::string Topic);
    bool Unsubscribe();

    // Other
    std::string MessageStatusToString(RdKafka::Message::Status Status);
    
private:
    
    Loger* loger;
    ErrorHandler* error;

    RdKafka::Producer* producer;
    RdKafka::KafkaConsumer* consumer;
    RdKafka::Message* message;    
    ConfigBuilder* config;
    MessageStatusCollector* messageStatusCollector;            
    
    bool FillHeaders(RdKafka::Headers* Headers, std::string StringHeaders);
};

