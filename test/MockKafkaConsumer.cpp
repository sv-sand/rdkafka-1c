#include "MockKafkaConsumer.h"

namespace Kafka1C
{
	std::string MockKafkaConsumer::name() const
	{
		return "MockKafkaConsumer";
	}

	std::string MockKafkaConsumer::memberid() const
	{
		return "";
	}

	int MockKafkaConsumer::poll(int)
	{
		return 0;
	}

	int MockKafkaConsumer::outq_len()
	{
		return 0;
	}

	RdKafka::ErrorCode MockKafkaConsumer::metadata(bool, const RdKafka::Topic*, RdKafka::Metadata**, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::pause(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::resume(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::query_watermark_offsets(const std::string&, int32_t, int64_t*, int64_t*, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::get_watermark_offsets(const std::string&, int32_t, int64_t*, int64_t*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::offsetsForTimes(std::vector<RdKafka::TopicPartition*>&, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::Queue* MockKafkaConsumer::get_partition_queue(const RdKafka::TopicPartition*)
	{
		return nullptr;
	}

	RdKafka::ErrorCode MockKafkaConsumer::set_log_queue(RdKafka::Queue*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	void MockKafkaConsumer::yield()
	{
	}

	std::string MockKafkaConsumer::clusterid(int)
	{
		return "";
	}

	struct rd_kafka_s* MockKafkaConsumer::c_ptr()
	{
		return nullptr;
	}

	int32_t MockKafkaConsumer::controllerid(int)
	{
		return -1;
	}

	RdKafka::ErrorCode MockKafkaConsumer::fatal_error(std::string&) const
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::oauthbearer_set_token(const std::string&, int64_t, const std::string&, const std::list<std::string>&, std::string&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::oauthbearer_set_token_failure(const std::string&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::Error* MockKafkaConsumer::sasl_background_callbacks_enable()
	{
		return nullptr;
	}

	RdKafka::Queue* MockKafkaConsumer::get_sasl_queue()
	{
		return nullptr;
	}

	RdKafka::Queue* MockKafkaConsumer::get_background_queue()
	{
		return nullptr;
	}

	void* MockKafkaConsumer::mem_malloc(size_t)
	{
		return nullptr;
	}

	void MockKafkaConsumer::mem_free(void*)
	{
	}

	RdKafka::Error* MockKafkaConsumer::sasl_set_credentials(const std::string&, const std::string&)
	{
		return nullptr;
	}

	RdKafka::ErrorCode MockKafkaConsumer::assignment(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::subscription(std::vector<std::string>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::subscribe(const std::vector<std::string>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::unsubscribe()
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::assign(const std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::unassign()
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitSync()
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitAsync()
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitSync(RdKafka::Message*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitAsync(RdKafka::Message*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitSync(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitAsync(const std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitSync(RdKafka::OffsetCommitCb*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::commitSync(std::vector<RdKafka::TopicPartition*>&, RdKafka::OffsetCommitCb*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::position(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::close()
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::seek(const RdKafka::TopicPartition&, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockKafkaConsumer::offsets_store(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ConsumerGroupMetadata* MockKafkaConsumer::groupMetadata()
	{
		return nullptr;
	}

	bool MockKafkaConsumer::assignment_lost()
	{
		return false;
	}

	std::string MockKafkaConsumer::rebalance_protocol()
	{
		return "";
	}

	RdKafka::Error* MockKafkaConsumer::incremental_assign(const std::vector<RdKafka::TopicPartition*>&)
	{
		return nullptr;
	}

	RdKafka::Error* MockKafkaConsumer::incremental_unassign(const std::vector<RdKafka::TopicPartition*>&)
	{
		return nullptr;
	}

	RdKafka::Error* MockKafkaConsumer::close(RdKafka::Queue*)
	{
		return nullptr;
	}

	bool MockKafkaConsumer::closed()
	{
		return false;
	}

} // namespace Kafka1C