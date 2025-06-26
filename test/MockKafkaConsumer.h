#pragma once

#include <librdkafka/rdkafkacpp.h>
#include <string>
#include <vector>
#include <gmock/gmock.h>

namespace Kafka1C
{
    class MockKafkaConsumer : 
        public RdKafka::KafkaConsumer 
    {
    public:
        MockKafkaConsumer() = default;
        ~MockKafkaConsumer() override = default;

        // Handle interface
        std::string name() const override;
        std::string memberid() const override;
        int poll(int timeout_ms) override;
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

        // KafkaConsumer interface
        RdKafka::ErrorCode assignment(std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::ErrorCode subscription(std::vector<std::string>& topics) override;
        RdKafka::ErrorCode subscribe(const std::vector<std::string>& topics) override;
        RdKafka::ErrorCode unsubscribe() override;
        RdKafka::ErrorCode assign(const std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::ErrorCode unassign() override;
        RdKafka::ErrorCode commitAsync() override;
        RdKafka::ErrorCode commitSync(RdKafka::Message* message) override;
        RdKafka::ErrorCode commitAsync(RdKafka::Message* message) override;
        RdKafka::ErrorCode commitAsync(const std::vector<RdKafka::TopicPartition*>& offsets) override;
        RdKafka::ErrorCode commitSync(RdKafka::OffsetCommitCb* offset_commit_cb) override;
        RdKafka::ErrorCode commitSync(std::vector<RdKafka::TopicPartition*>& offsets, RdKafka::OffsetCommitCb* offset_commit_cb) override;
        RdKafka::ErrorCode position(std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::ErrorCode close() override;
        RdKafka::ErrorCode seek(const RdKafka::TopicPartition& partition, int timeout_ms) override;
        RdKafka::ErrorCode offsets_store(std::vector<RdKafka::TopicPartition*>& offsets) override;
        RdKafka::ConsumerGroupMetadata* groupMetadata() override;
        bool assignment_lost() override;
        std::string rebalance_protocol() override;
        RdKafka::Error* incremental_assign(const std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::Error* incremental_unassign(const std::vector<RdKafka::TopicPartition*>& partitions) override;
        RdKafka::Error* close(RdKafka::Queue* queue) override;
        bool closed() override;

        MOCK_METHOD(int, outq_len, (), (override));
        MOCK_METHOD(RdKafka::Message*, consume, (int), (override));
        MOCK_METHOD(RdKafka::ErrorCode, commitSync, (), (override));
        MOCK_METHOD(RdKafka::ErrorCode, commitSync, (std::vector<RdKafka::TopicPartition*>&), (override));
        MOCK_METHOD(RdKafka::ErrorCode, committed, (std::vector<RdKafka::TopicPartition*>&, int), (override));
    };
} // namespace Kafka1C