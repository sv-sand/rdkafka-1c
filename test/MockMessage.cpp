#include "MockMessage.h"

namespace Kafka1C {

    // Interface methods
    std::string MockMessage::errstr() const
    {
        return "";
    }

    RdKafka::ErrorCode MockMessage::err() const
    {
        return error_;
    }

    RdKafka::Topic* MockMessage::topic() const
    {
        return nullptr;
    }

    std::string MockMessage::topic_name() const
    {
        return topic_name_;
    }

    int32_t MockMessage::partition() const
    {
        return partition_;
    }

    void* MockMessage::payload() const
    {
        return const_cast<void*>(static_cast<const void*>(payload_.data()));
    }

    size_t MockMessage::len() const
    {
        return payload_.length();
    }

    const std::string* MockMessage::key() const
    {
        return &key_;
    }

    const void* MockMessage::key_pointer() const
    {
        return &key_;
    }

    size_t MockMessage::key_len() const
    {
        return key_.length();
    }

    int64_t MockMessage::offset() const
    {
        return offset_;
    }

    void* MockMessage::msg_opaque() const
    {
        return nullptr;
    }

    int64_t MockMessage::latency() const
    {
        return 0;
    }

    rd_kafka_message_s* MockMessage::c_ptr()
    {
        return nullptr;
    }

    RdKafka::MessageTimestamp MockMessage::timestamp() const
    {
        return timestamp_;
    }

    RdKafka::Headers* MockMessage::headers()
    {
        return headers_;
    }

    RdKafka::Headers* MockMessage::headers(RdKafka::ErrorCode* err)
    {
        return headers_;
    }

    int32_t MockMessage::broker_id() const
    {
        return 0;
    }

    int32_t MockMessage::leader_epoch() const
    {
        return 0;
    }

    RdKafka::Error* MockMessage::offset_store()
    {
        return nullptr;
    }

    RdKafka::Message::Status MockMessage::status() const
    {
        return Status::MSG_STATUS_PERSISTED;
    }
}