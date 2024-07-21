#include "Config.h"

Config::Config(Loger* Loger)
{
    loger = Loger;
    
    loger->Info("Create global config");
    config = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    loger->Info("Delivery report callback initialization");
    deliveryReport = new DeliveryReport();
    deliveryReport->SetLoger(loger);

    loger->Info("Rebalance callback initialization");
    rebalance = new Rebalance();
    rebalance->SetLoger(loger);

    loger->Info("Event callback initialization");
    event = new Event();
    event->SetLoger(loger);
}

Config::~Config()
{
    delete deliveryReport;
    delete rebalance;
    delete event;
    delete config;
}

bool Config::Build(std::string& ErrorDescription)
{
    // Set common props
    if (loger->level == Loger::Levels::DEBUG)
        if (!Set("debug", "all", ErrorDescription))
            return false;

    if (!Set("enable.partition.eof", "true", ErrorDescription))
        return false;

    // Set user props
    for (const auto& [key, value] : properties)
        if (!Set(key, value, ErrorDescription))
            return false;

    // Set callbacks
    if (!SetDeliveryReport(deliveryReport, ErrorDescription))
        return false;

    if (!SetRebalance(rebalance, ErrorDescription))
        return false;

    if (!SetEvent(event, ErrorDescription))
        return false;

    // Log dump
    if (loger->level == Loger::Levels::DEBUG)
        LogConfigDump();

    return true;
}

RdKafka::Conf* Config::Get()
{
    return config;
}

void Config::AddProperty(std::string Name, std::string Value)
{
    properties[Name] = Value;
}

void Config::ClearProperties()
{
    properties.clear();
}

/////////////////////////////////////////////////////////////////////////////
// Support methods

void Config::LogConfigDump()
{
    std::list<std::string>* dump = config->dump();
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

bool Config::Set(std::string Name, std::string Value, std::string& ErrorDescription)
{
    loger->Info("Set config property '" + Name + "' in value '" + Value + "'");

    RdKafka::Conf::ConfResult result = config->set(Name, Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

bool Config::SetDeliveryReport(DeliveryReport* Value, std::string& ErrorDescription)
{
    loger->Info("Set delivery report in config property 'dr_cb'");

    RdKafka::Conf::ConfResult result = config->set("dr_cb", Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

bool Config::SetRebalance(Rebalance* Value, std::string& ErrorDescription)
{
    loger->Info("Set rebalance callback in config property 'rebalance_cb'");

    RdKafka::Conf::ConfResult result = config->set("rebalance_cb", Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

bool Config::SetEvent(Event* Value, std::string& ErrorDescription)
{
    loger->Info("Set event callback in config property 'event_cb'");

    RdKafka::Conf::ConfResult result = config->set("event_cb", Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

