#pragma once

#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include "Loger.h"

namespace RdKafka1C {

    class Rebalance : public RdKafka::RebalanceCb {
    public:

        Rebalance(Loger* Loger);
        void rebalance_cb(RdKafka::KafkaConsumer* consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions);

    private:
        Loger* loger;

        void LogConfig(const std::vector<RdKafka::TopicPartition*>& partitions);
        void CooperativeRebalance(RdKafka::KafkaConsumer* consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions);
        void EagerRebalance(RdKafka::KafkaConsumer* consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions);
    };

}