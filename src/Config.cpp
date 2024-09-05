#include "Config.h"

Config::Config(Loger* Loger)
{
    loger = Loger;

    Rebalance = nullptr;
    Event = nullptr;
    DeliveryReport = nullptr;
    
    loger->Info("Create config");
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
}

Config::~Config()
{
}

bool Config::Build(Purposes Purpose, std::string& ErrorDescription)
{
    loger->Info("Build config");
    
    loger->Debug("Set common props");
    if (loger->level == Loger::Levels::DEBUG)
        if (!SetProperty("debug", "all", ErrorDescription))
            return false;

    if (!SetProperty("client.id", "rdkafka-1c", ErrorDescription))
        return false;

    if (Rebalance && Purpose == Purposes::CONSUME)
        if (!SetProperty("enable.partition.eof", "true", ErrorDescription))
            return false;

    loger->Debug("Set user props");
    for (const auto& [key, value] : properties)
        if (!SetProperty(key, value, ErrorDescription))
            return false;

    loger->Debug("Set callbacks");

    if (Event)
    {
        loger->Debug("Set event callback in config property 'event_cb'");
        RdKafka::Conf::ConfResult result = conf->set("event_cb", Event, ErrorDescription);
        if (result != RdKafka::Conf::CONF_OK)
        {
            loger->Error("Failed to set config property: " + ErrorDescription);
            return false;
        }
    }
    
    if (DeliveryReport && Purpose == Purposes::PRODUCE)
    {
        loger->Debug("Set delivery report in config property 'dr_cb'");
        RdKafka::Conf::ConfResult result = conf->set("dr_cb", DeliveryReport, ErrorDescription);
        if (result != RdKafka::Conf::CONF_OK)
        {
            loger->Error("Failed to set config property: " + ErrorDescription);
            return false;
        }
    }    
    
    if (Rebalance && Purpose == Purposes::CONSUME)
    {
        loger->Debug("Set rebalance callback in config property 'rebalance_cb'");

        RdKafka::Conf::ConfResult result = conf->set("rebalance_cb", Rebalance, ErrorDescription);
        if (result != RdKafka::Conf::CONF_OK)
        {
            loger->Error("Failed to set config property: " + ErrorDescription);
            return false;
        }
    }
    
    if (loger->level == Loger::Levels::DEBUG)
        LogConfigDump();

    loger->Debug("Clear config properties after build");
    properties.clear();
    
    return true;
}

RdKafka::Conf* Config::GetConf()
{
    return conf;
}

void Config::AddProperty(std::string Name, std::string Value)
{
    properties[Name] = Value;
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
    loger->Debug("Set config property '" + Name + "' in value '" + Value + "'");

    RdKafka::Conf::ConfResult result = conf->set(Name, Value, ErrorDescription);
    if (result != RdKafka::Conf::CONF_OK)
    {
        loger->Error("Failed to set config property: " + ErrorDescription);
        return false;
    }

    return true;
}

