#pragma once

#include <sstream>
#include <map>
#include "Loger.h"
#include "DeliveryReport.h"
#include "Rebalance.h"
#include "Event.h" 

class Config
{
public:
    
    enum class Purposes {
        PRODUCE,
        CONSUME
    };

    Config(Loger* Loger);
    ~Config();

    DeliveryReport* deliveryReport;
    Rebalance* rebalance;
    Event* event;
    
    void AddProperty(std::string Name, std::string Value);
    bool Build(Purposes Purpose, std::string& ErrorDescription);    
    RdKafka::Conf* GetConf();    
    
private:
    RdKafka::Conf* conf;   
    Loger* loger;
    std::map<std::string, std::string> properties;

    void LogConfigDump();

    bool SetProperty(std::string Name, std::string Value, std::string& ErrorDescription);
};