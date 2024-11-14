#include "MessageStatusCollector.h"

void MessageStatusCollector::Add(std::string uuid, RdKafka::Message::Status status)
{
    collector[uuid] = status;
}

RdKafka::Message::Status MessageStatusCollector::Get(std::string Uuid)
{
    RdKafka::Message::Status status;

    try
    {
        status = collector.at(Uuid);
    }
    catch (const std::out_of_range& e)
    {
        status = RdKafka::Message::MSG_STATUS_NOT_PERSISTED;
    }

    return status;
}

void MessageStatusCollector::Clear()
{
    collector.clear();
}

int MessageStatusCollector::CountUndelivered()
{
    int count = 0;
    
    for (const auto& [key, value] : collector)
        if (value != RdKafka::Message::MSG_STATUS_PERSISTED)
            count++;

    return count;
}
