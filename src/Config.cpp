#include "Config.h"

Config::Config(Loger* Loger)
{
    loger = Loger;

    rebalance = nullptr;
    event = nullptr;
    deliveryReport = nullptr;
    
    loger->Info("Create global config");
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
}

Config::~Config()
{
    
}

bool Config::Build(std::string& ErrorDescription)
{
    // Set common props
    if (loger->level == Loger::Levels::DEBUG)
        if (!SetProperty("debug", "all", ErrorDescription))
            return false;

    if (!SetProperty("client.id", "rdkafka-1c", ErrorDescription))
        return false;

    if (!SetProperty("enable.partition.eof", "true", ErrorDescription))
        return false;

    // Set user props
    for (const auto& [key, value] : properties)
        if (!SetProperty(key, value, ErrorDescription))
            return false;

    // Set callbacks
    if (!SetPropertyDeliveryReport(deliveryReport, ErrorDescription))
        return false;
    
    if (!SetPropertyRebalance(rebalance, ErrorDescription))
        return false;
        
    if (!SetPropertyEvent(event, ErrorDescription))
        return false;

    // Log dump
    if (loger->level == Loger::Levels::DEBUG)
        LogConfigDump();

    return true;
}

RdKafka::Conf* Config::GetConf()
{
    return conf;
}

void Config::ClearProperties()
{
    properties.clear();
}

void Config::AddProperty(std::string Name, std::string Value)
{
    properties[Name] = Value;
}

void Config::SetDeliveryReport(DeliveryReport* DeliveryReport)
{
    deliveryReport = DeliveryReport;
}

void Config::SetRebalance(Rebalance* Rebalance)
{
    rebalance = Rebalance;
}

void Config::SetEvent(Event* Event)
{
    event = Event;
}

/////////////////////////////////////////////////////////////////////////////
// Support methods

void Config::LogConfigDump()
{
    std::list<std::string>* dump = conf->dump();
    std::stringstream stream;

    stream << "Config dump:";

    for (auto it = dump->begin(); it != dump->end();)
    {
        stream << std::endl << *it << " = ";
        it++;
        stream << *it;
        it++;
    }
    loger->Debug(stream.str());
}

bool Config::SetProperty(std::string Name, std::string Value, std::string& ErrorDescription)
{
    loger->Info("Set config property '" + Name + "' in value '" + Value + "'");

    RdKafka::Conf::ConfResult result = conf->set(Name, Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

bool Config::SetPropertyDeliveryReport(DeliveryReport* Value, std::string& ErrorDescription)
{
    if (!deliveryReport)
        return true;
    
    loger->Info("Set delivery report in config property 'dr_cb'");

    RdKafka::Conf::ConfResult result = conf->set("dr_cb", Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

bool Config::SetPropertyRebalance(Rebalance* Value, std::string& ErrorDescription)
{
    if (!rebalance)
        return true;
    
    loger->Info("Set rebalance callback in config property 'rebalance_cb'");

    RdKafka::Conf::ConfResult result = conf->set("rebalance_cb", Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

bool Config::SetPropertyEvent(Event* Value, std::string& ErrorDescription)
{
    if (!event)
        return true;
    
    loger->Info("Set event callback in config property 'event_cb'");

    RdKafka::Conf::ConfResult result = conf->set("event_cb", Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

