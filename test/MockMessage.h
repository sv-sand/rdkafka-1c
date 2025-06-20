#pragma once

#include "librdkafka/rdkafkacpp.h"
#include <string>
#include <memory>

namespace RdKafka1C {

    class MockMessage :
        public RdKafka::Message
    {
    public:
        
		// Interface methods
        std::string errstr() const override;
        RdKafka::ErrorCode err() const override;
        RdKafka::Topic* topic() const override;
        std::string topic_name() const override;
        int32_t partition() const override;
        void* payload() const override;
        size_t len() const override;
        const std::string* key() const override;
		const void* key_pointer() const override;
        size_t key_len() const override;
        int64_t offset() const override;
        RdKafka::MessageTimestamp timestamp() const override;
        void* msg_opaque() const override;
		int64_t latency() const override;
		struct rd_kafka_message_s* c_ptr() override;
        Status status() const override;
        RdKafka::Headers* headers() override;
		RdKafka::Headers* headers(RdKafka::ErrorCode* err) override;
		int32_t broker_id() const override;
		int32_t leader_epoch() const override;
        RdKafka::Error* offset_store() override;
        
        // Message data for testing
        RdKafka::ErrorCode error_ = RdKafka::ERR_NO_ERROR;
        std::string topic_name_ = "";
        int32_t partition_ = 0;
        int64_t offset_ = 0;
        std::string payload_ = "";
        std::string key_ = "";
        
        RdKafka::MessageTimestamp timestamp_{ RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE , 0};
        RdKafka::Headers* headers_ = RdKafka::Headers::create();
    };

} // namespace RdKafka1C