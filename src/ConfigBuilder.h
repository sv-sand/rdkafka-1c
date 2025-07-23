#pragma once

#include <sstream>
#include <map>
#include "Loger.h"
#include "ErrorHandler.h"
#include "DeliveryReport.h"
#include "Rebalance.h"
#include "Event.h" 
#include "utils.h"

namespace Kafka1C {

    class ConfigBuilder {

    public:
        ConfigBuilder(Loger* Loger, ErrorHandler* Error);
        ~ConfigBuilder();

        void AddProperty(std::string Name, std::string Value);
        bool BuildProducerConfig();
        bool BuildConsumerConfig();
        RdKafka::Conf* GetConf();

        DeliveryReport* GetDeliveryReport();

    private:
        Loger* loger;
        ErrorHandler* error;
        RdKafka::Conf* conf;
        Rebalance* rebalance;
        Event* event;
        DeliveryReport* deliveryReport;

        std::map<std::string, std::string> properties;

        void LogConfigDump();
        bool SetProperty(std::string Name, std::string Value);
        bool SetEventCb();
        bool SetDeliveryReportCb();
        bool SetRebalanceCb();
    };

} // namespace RdKafka1C