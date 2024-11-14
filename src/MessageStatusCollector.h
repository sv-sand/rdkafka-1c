#pragma once

#include <map>
#include <stdexcept>
#include <librdkafka/rdkafkacpp.h>

class MessageStatusCollector
{
    public:
        void Add(std::string uuid, RdKafka::Message::Status status);
        RdKafka::Message::Status Get(std::string Uuid);
        void Clear();

        int CountUndelivered();

    private:
        std::map<std::string, RdKafka::Message::Status> collector; 
};