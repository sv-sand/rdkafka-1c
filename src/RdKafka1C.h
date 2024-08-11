#pragma once

#ifdef WIN32
#pragma comment(lib, "bcrypt.lib")
#endif

#include <map>
#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid.hpp>           
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include "Config.h"
#include "Loger.h"

#define delete_pointer(ptr) {delete ptr; ptr = nullptr;}

class RdKafka1C
{
public:

    int OperationTimeout = 1000; // ms

    RdKafka1C();
    ~RdKafka1C();

    std::string RdKafkaVersion();
    std::string ErrorDescription();

    // Set RdKafka config property https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md
    // Invoke this method before InitProducer(), InitConsumer()
    void SetConfigProperty(std::string Name, std::string Value);

    // Producer
    bool InitProducer(std::string Brokers, std::string Topic);
    bool Produce(std::string Message);
    bool Produce(std::string Message, std::string Headers);
    bool Produce(std::string Message, std::string Key, std::string Headers, int partition);
    bool StopProduser();

    // Consumer
    bool InitConsumer(std::string Brokers, std::string GroupId);
    bool Subscribe(std::string Topic);
    bool Consume();
    std::string GetMessageData();
    std::string GetMessageMetadata();
    bool CommitOffset(std::string Topic, int Offcet, int Partition);
    bool Unsubscribe();
    bool StopConsumer();

    // Logging
    bool StartLogging(std::string Directory);
    bool StartLogging(std::string Directory, Loger::Levels Level);
    void StopLogging();
    Loger::Levels GetLogerLevel();
    void SetLogerLevel(Loger::Levels Level);
    std::string GetCurrentLogFile();
    
private:
    
    RdKafka::Producer* producer;
    RdKafka::KafkaConsumer* consumer;
    RdKafka::Message* message;
    std::string topicName;
    
    Config* config;
    Rebalance* rebalance;
    Event* event;
    DeliveryReport* deliveryReport;

    Loger* loger;
    std::string errorDescription;

    RdKafka::Headers* CreateHeaders(std::string Header);

};

