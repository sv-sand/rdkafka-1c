#pragma once

#include <map>
#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "Config.h"
#include "Loger.h"

class RdKafka1C
{
public:

    int OperationTimeout = 1000; // ms

    RdKafka1C();
    ~RdKafka1C();

    std::string RdKafkaVersion();
    std::string ErrorDescription();

    bool InitLoger(std::string Directory);
    bool InitLoger(std::string Directory, Loger::Levels Level);
    Loger::Levels GetLogerLevel();
    void SetLogerLevel(Loger::Levels Level);
    std::string GetCurrentLogFile();

    // Set RdKafka config property https://github.com/confluentinc/librdkafka/blob/master/CONFIGURATION.md
    // Invoke this method before InitProducer(), InitConsumer()
    void SetConfigProperty(std::string Name, std::string Value);

    bool InitProducer(std::string Brokers, std::string Topic);
    bool Produce(std::string Message);
    bool Produce(std::string Message, std::string Headers);
    bool Produce(std::string Message, std::string Headers, int partition);
    bool StopProduser();

    bool InitConsumer(std::string Brokers, std::string Topic, std::string GroupId);
    bool Consume();
    std::string GetMessageData();
    std::string GetMessageMetadata();
    bool StopConsumer();
    
private:
    
    Config* config;
    RdKafka::Producer* producer;
    RdKafka::KafkaConsumer* consumer;
    RdKafka::Message* message;
    std::string topicName;
    
    Loger* loger;
    std::string errorDescription;

    RdKafka::Headers* CreateHeaders(std::string Header);

};

