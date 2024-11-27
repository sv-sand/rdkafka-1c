#include "ConfigBuilder.h"

ConfigBuilder::ConfigBuilder(Loger* Loger, ErrorHandler* Error) {
    loger = Loger;
    error = Error;

    rebalance = new Rebalance(loger);
    event = new Event(loger);
    deliveryReport = new DeliveryReport(loger);
    
    loger->Info("Create config");
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
}

ConfigBuilder::~ConfigBuilder() {
    delete_pointer(conf);
    delete_pointer(rebalance);
    delete_pointer(event);
    delete_pointer(deliveryReport);
}

bool ConfigBuilder::BuildProducerConfig() {
    loger->Info("Build config");
    
    loger->Debug("Set common props");
    if (loger->level == Loger::Levels::DEBUG)
        if (!SetProperty("debug", "all"))
            return false;

    if (!SetProperty("client.id", "rdkafka-1c"))
        return false;

    loger->Debug("Set user props");
    for (const auto& [key, value] : properties)
        if (!SetProperty(key, value))
            return false;

    loger->Debug("Set callbacks");
    if (!SetEventCb())
        return false;

    if (!SetDeliveryReportCb())
        return false; 
    
    if (loger->level == Loger::Levels::DEBUG)
        LogConfigDump();

    loger->Debug("Clear config properties after build");
    properties.clear();
    
    return true;
}

bool ConfigBuilder::BuildConsumerConfig() {
    loger->Info("Build config");
    
    loger->Debug("Set common props");
    if (loger->level == Loger::Levels::DEBUG)
        if (!SetProperty("debug", "all"))
            return false;

    if (!SetProperty("client.id", "rdkafka-1c"))
        return false;

    if (rebalance)
        if (!SetProperty("enable.partition.eof", "true"))
            return false;

    loger->Debug("Set user props");
    for (const auto& [key, value] : properties)
        if (!SetProperty(key, value))
            return false;

    loger->Debug("Set callbacks");
    if (!SetEventCb())
        return false;
    
    if (!SetRebalanceCb())
        return false;
        
    if (loger->level == Loger::Levels::DEBUG)
        LogConfigDump();

    loger->Debug("Clear config properties after build");
    properties.clear();
    
    return true;
}

RdKafka::Conf* ConfigBuilder::GetConf() {
    return conf;
}

void ConfigBuilder::AddProperty(std::string Name, std::string Value) {
    properties[Name] = Value;
}

DeliveryReport* ConfigBuilder::GetDeliveryReport() {
    return deliveryReport;
}

/////////////////////////////////////////////////////////////////////////////
// Support methods

void ConfigBuilder::LogConfigDump() {
    std::list<std::string>* dump = conf->dump();
    std::stringstream stream;

    stream << "Config dump:";

    for (auto it = dump->begin(); it != dump->end();) {
        stream << std::endl << *it << " = ";
        it++;
        stream << *it;
        it++;
    }
    loger->Debug(stream.str());
}

bool ConfigBuilder::SetProperty(std::string Name, std::string Value) {
    loger->Debug("Set config property '" + Name + "' in value '" + Value + "'");

    std::string errorDescription;
    RdKafka::Conf::ConfResult result = conf->set(Name, Value, errorDescription);
    if (result != RdKafka::Conf::CONF_OK) {
        loger->Error("Failed to set config property: " + errorDescription);
        return false;
    }

    return true;
}

bool ConfigBuilder::SetEventCb() {
    if (!event)
        return true;
    
    loger->Debug("Set config property 'event_cb'");

    std::string errorDescription;
    RdKafka::Conf::ConfResult result = conf->set("event_cb", event, errorDescription);
    if (result != RdKafka::Conf::CONF_OK) {
        loger->Error("Failed to set config property 'event_cb': " + errorDescription);
        return false;
    }

    return true;
}

bool ConfigBuilder::SetDeliveryReportCb() {
    if (!deliveryReport)
        return true;
    
    loger->Debug("Set config property 'dr_cb'");

    std::string errorDescription;
    RdKafka::Conf::ConfResult result = conf->set("dr_cb", deliveryReport, errorDescription);
    if (result != RdKafka::Conf::CONF_OK) {
        loger->Error("Failed to set config property 'dr_cb': " + errorDescription);
        return false;
    }

    return true;
}

bool ConfigBuilder::SetRebalanceCb() {
    if (!rebalance)
        return true;
    
    loger->Debug("Set config property 'rebalance_cb'");

    std::string errorDescription;
    RdKafka::Conf::ConfResult result = conf->set("rebalance_cb", rebalance, errorDescription);
    if (result != RdKafka::Conf::CONF_OK) {
        loger->Error("Failed to set config property 'rebalance_cb': " + errorDescription);
        return false;
    }

    return true;
}
