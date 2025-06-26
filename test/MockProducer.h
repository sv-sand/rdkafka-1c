#pragma once

#include <librdkafka/rdkafkacpp.h>
#include <string>
#include <vector>
#include <gmock/gmock.h>

namespace Kafka1C
{
    class MockProducer :
        public RdKafka::Producer
    {
    public:
        MockProducer() = default;
        ~MockProducer() override = default;

        // Handle interface
        std::string name() const override;
        std::string memberid() const override;
        int poll(int timeout_ms) override;
        int outq_len() override;
        RdKafka::ErrorCode metadata(bool all_topics, const RdKafka::Topic* only_rkt, RdKafka::Metadata** metadatap, int timeout_ms) override;
        RdKafka::ErrorCode pause(std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::ErrorCode resume(std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::ErrorCode query_watermark_offsets(const std::string& topic, int32_t partition, int64_t* low, int64_t* high, int timeout_ms) override;
        RdKafka::ErrorCode get_watermark_offsets(const std::string& topic, int32_t partition, int64_t* low, int64_t* high) override;
        RdKafka::ErrorCode offsetsForTimes(std::vector<RdKafka::TopicPartition*>& offsets, int timeout_ms) override;
        RdKafka::Queue* get_partition_queue(const RdKafka::TopicPartition* partition) override;
        RdKafka::ErrorCode set_log_queue(RdKafka::Queue* queue) override;
        void yield() override;
        std::string clusterid(int timeout_ms) override;
        struct rd_kafka_s* c_ptr() override;
        int32_t controllerid(int timeout_ms) override;
        RdKafka::ErrorCode fatal_error(std::string& errstr) const override;
        RdKafka::ErrorCode oauthbearer_set_token(const std::string& token_value, int64_t md_lifetime_ms, const std::string& md_principal_name, const std::list<std::string>& extensions, std::string& errstr) override;
        RdKafka::ErrorCode oauthbearer_set_token_failure(const std::string& errstr) override;
        RdKafka::Error* sasl_background_callbacks_enable() override;
        RdKafka::Queue* get_sasl_queue() override;
        RdKafka::Queue* get_background_queue() override;
        void* mem_malloc(size_t size) override;
        void mem_free(void* ptr) override;
        RdKafka::Error* sasl_set_credentials(const std::string& username, const std::string& password) override;

        // Producer interface
        RdKafka::ErrorCode produce(RdKafka::Topic* topic, int32_t partition, int msgflags, void* payload, size_t len, const std::string* key, void* msg_opaque) override;
        RdKafka::ErrorCode produce(RdKafka::Topic* topic, int32_t partition, int msgflags, void* payload, size_t len, const void* key, size_t key_len, void* msg_opaque) override;
        RdKafka::ErrorCode produce(const std::string topic_name, int32_t partition, int msgflags, void* payload, size_t len, const void* key, size_t key_len, int64_t timestamp, void* msg_opaque) override;
        RdKafka::ErrorCode produce(const std::string topic_name, int32_t partition, int msgflags, void* payload, size_t len, const void* key, size_t key_len, int64_t timestamp, RdKafka::Headers* headers, void* msg_opaque) override;
        RdKafka::ErrorCode produce(RdKafka::Topic* topic, int32_t partition, const std::vector<char>* payload, const std::vector<char>* key, void* msg_opaque) override;
        RdKafka::ErrorCode purge(int purge_flags) override;
        RdKafka::Error* init_transactions(int timeout_ms) override;
        RdKafka::Error* begin_transaction() override;
        RdKafka::Error* send_offsets_to_transaction(const std::vector<RdKafka::TopicPartition*>& offsets, const RdKafka::ConsumerGroupMetadata* group_metadata, int timeout_ms) override;
        RdKafka::Error* commit_transaction(int timeout_ms) override;
        RdKafka::Error* abort_transaction(int timeout_ms) override;

        MOCK_METHOD(RdKafka::ErrorCode, flush, (int), (override));
    };

} // namespace Kafka1C