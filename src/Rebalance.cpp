#include "Rebalance.h"

/*
 * Rebalance.
 * The process of redistributing partitions among consumer instances in a group. Partition assignment strategy.
 * The algorithm used by Kafka to assign partitions to consumers in a group.
 */

Rebalance::Rebalance(Loger* Loger)
{
    loger = Loger;
}

void Rebalance::LogConfig(const std::vector<RdKafka::TopicPartition*>& partitions)
{
    std::stringstream stream;
    stream << "Partition config: ";

    for (unsigned int i = 0; i < partitions.size(); i++)
        stream << partitions[i]->topic() << "[" << partitions[i]->partition() << "], ";

    loger->Debug(stream.str());
}

void Rebalance::rebalance_cb(RdKafka::KafkaConsumer* consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions)
{
    loger->Debug("Rebalance: " + RdKafka::err2str(err));
        
    LogConfig(partitions);

    if (consumer->rebalance_protocol() == "COOPERATIVE")
        CooperativeRebalance(consumer, err, partitions);
    else
        EagerRebalance(consumer, err, partitions);
}

void Rebalance::CooperativeRebalance(RdKafka::KafkaConsumer* consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions)
{
    RdKafka::Error* result = NULL;
    
    switch (err)
    {
    case RdKafka::ERR__ASSIGN_PARTITIONS:
        // Application may load offets from arbitrary external
        // storage here and update partitions
        result = consumer->incremental_assign(partitions);
        break;

    case RdKafka::ERR__REVOKE_PARTITIONS:
        // Application may commit offsets manually here
        // if auto.commit.enable=false
        result = consumer->incremental_unassign(partitions);
        break;

    default:
        result = consumer->incremental_unassign(partitions);
    }

    if (result)
        loger->Error("Incremental assign failed: " + result->str());

    delete result;
}

void Rebalance::EagerRebalance(RdKafka::KafkaConsumer* consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*>& partitions)
{
    RdKafka::ErrorCode result = RdKafka::ERR_NO_ERROR;

    switch (err)
    {
    case RdKafka::ERR__ASSIGN_PARTITIONS:
        // Application may load offets from arbitrary external
        // storage here and update partitions
        result = consumer->assign(partitions);
        break;

    case RdKafka::ERR__REVOKE_PARTITIONS:
        // Application may commit offsets manually here
        // if auto.commit.enable=false
        result = consumer->unassign();
        break;

    default:
        result = consumer->unassign();
    }

    if (result)
        loger->Error("Assign failed: " + RdKafka::err2str(result));
}
