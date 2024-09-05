#pragma once

#ifdef WIN32
#pragma comment(lib, "bcrypt.lib")
#endif

#include <map>
#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "Config.h"
#include "Loger.h"
#include "Strings.h"

#define delete_pointer(ptr) {delete ptr; ptr = nullptr;}

class RdKafka1C
{
public:

    int OperationTimeout = 10000; // ms

    RdKafka1C();
    ~RdKafka1C();

    std::string RdKafkaVersion();
    std::string ErrorDescription();

    // Set RdKafka config property https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md
    // Invoke this method before InitProducer(), InitConsumer()
    void SetConfigProperty(std::string Name, std::string Value);

    // Producer
    bool InitProducer(std::string Brokers);
    bool Produce(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition);
    bool StartProduceAsynch();
    bool ProduceAsynch(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition);
    bool Flush();
    bool StopProduser();
    int ProducerQueueLen();

    // Consumer
    bool InitConsumer(std::string Brokers, std::string GroupId);
    bool Consume();
    std::string GetMessageData();
    std::string GetMessageMetadata();
    bool CommitOffset(std::string Topic, int Partition, int64_t Offset);
    bool StopConsumer();
    int ConsumerQueueLen();

    // Subscriptions
    std::string Subscription();
    bool Subscribe(std::string Topic);
    bool Unsubscribe();

    // Logging
    bool StartLogging(std::string FileName);
    bool StartLogging(std::string FileName, Loger::Levels Level);
    void StopLogging();
    Loger::Levels GetLogerLevel();
    void SetLogerLevel(Loger::Levels Level);
    std::string GetCurrentLogFile();

    // Other
    std::string MessageStatusToString(RdKafka::Message::Status Status);
    
private:
    
    RdKafka::Producer* producer;
    RdKafka::KafkaConsumer* consumer;
    RdKafka::Message* message;
    
    Config* config;
    Rebalance* rebalance;
    Event* event;
    DeliveryReport* deliveryReport;

    Loger* loger;
    std::string errorDescription;

    bool CreateHeaders(RdKafka::Headers* Headers, std::string StringHeaders);
};

