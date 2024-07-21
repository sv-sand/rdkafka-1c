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
    
    Config(Loger* Loger);
    ~Config();

    bool Build(std::string& ErrorDescription);    
    RdKafka::Conf* Get();    
    
    void AddProperty(std::string Name, std::string Value);
    void ClearProperties();

private:
    RdKafka::Conf* config;

    Rebalance* rebalance;
    Event* event;
    DeliveryReport* deliveryReport;

    Loger* loger;
    std::map<std::string, std::string> properties;

    void LogConfigDump();

    bool Set(std::string Name, std::string Value, std::string& ErrorDescription);
    bool SetDeliveryReport(DeliveryReport* Value, std::string& ErrorDescription);
    bool SetRebalance(Rebalance* Value, std::string& ErrorDescription);
    bool SetEvent(Event* Value, std::string& ErrorDescription);
};