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
    RdKafka::Conf* GetConf();    
    
    void ClearProperties();
    void AddProperty(std::string Name, std::string Value);
    void SetDeliveryReport(DeliveryReport* DeliveryReport);
    void SetRebalance(Rebalance* Rebalance);
    void SetEvent(Event* Event);

private:
    RdKafka::Conf* conf;
    DeliveryReport* deliveryReport;
    Rebalance* rebalance;
    Event* event;
    
    Loger* loger;
    std::map<std::string, std::string> properties;

    void LogConfigDump();

    bool SetProperty(std::string Name, std::string Value, std::string& ErrorDescription);
    bool SetPropertyDeliveryReport(DeliveryReport* Value, std::string& ErrorDescription);
    bool SetPropertyRebalance(Rebalance* Value, std::string& ErrorDescription);
    bool SetPropertyEvent(Event* Value, std::string& ErrorDescription);
};