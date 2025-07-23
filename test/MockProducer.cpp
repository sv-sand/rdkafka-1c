#include "MockProducer.h"

namespace Kafka1C
{
	std::string MockProducer::name() const
	{
		return "MockProducer";
	}

	std::string MockProducer::memberid() const
	{
		return "";
	}

	int MockProducer::poll(int)
	{
		return 0;
	}

	int MockProducer::outq_len()
	{
		return 0;
	}

	RdKafka::ErrorCode MockProducer::metadata(bool, const RdKafka::Topic*, RdKafka::Metadata**, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::pause(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::resume(std::vector<RdKafka::TopicPartition*>&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::query_watermark_offsets(const std::string&, int32_t, int64_t*, int64_t*, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::get_watermark_offsets(const std::string&, int32_t, int64_t*, int64_t*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::offsetsForTimes(std::vector<RdKafka::TopicPartition*>&, int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::Queue* MockProducer::get_partition_queue(const RdKafka::TopicPartition*)
	{
		return nullptr;
	}

	RdKafka::ErrorCode MockProducer::set_log_queue(RdKafka::Queue*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	void MockProducer::yield()
	{
	}

	std::string MockProducer::clusterid(int)
	{
		return "";
	}

	struct rd_kafka_s* MockProducer::c_ptr()
	{
		return nullptr;
	}

	int32_t MockProducer::controllerid(int)
	{
		return -1;
	}

	RdKafka::ErrorCode MockProducer::fatal_error(std::string&) const
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::oauthbearer_set_token(
		const std::string&, int64_t, const std::string&, const std::list<std::string>&, std::string&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::oauthbearer_set_token_failure(const std::string&)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::Error* MockProducer::sasl_background_callbacks_enable()
	{
		return nullptr;
	}

	RdKafka::Queue* MockProducer::get_sasl_queue()
	{
		return nullptr;
	}

	RdKafka::Queue* MockProducer::get_background_queue() { return nullptr; }

	void* MockProducer::mem_malloc(size_t)
	{
		return nullptr;
	}

	void MockProducer::mem_free(void*)
	{
	}

	RdKafka::Error* MockProducer::sasl_set_credentials(const std::string&, const std::string&)
	{
		return nullptr;
	}

	RdKafka::ErrorCode MockProducer::produce(RdKafka::Topic*, int32_t, int, void*, size_t, const std::string*, void*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::produce(RdKafka::Topic*, int32_t, int, void*, size_t, const void*, size_t, void*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::produce(const std::string, int32_t, int, void*, size_t, const void*, size_t, int64_t, void*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::produce(
		const std::string, int32_t, int, void*, size_t, const void*, size_t, int64_t, RdKafka::Headers*, void*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::produce(RdKafka::Topic*, int32_t, const std::vector<char>*, const std::vector<char>*, void*)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::ErrorCode MockProducer::purge(int)
	{
		return RdKafka::ERR_NO_ERROR;
	}

	RdKafka::Error* MockProducer::init_transactions(int)
	{
		return nullptr;
	}

	RdKafka::Error* MockProducer::begin_transaction()
	{
		return nullptr;
	}

	RdKafka::Error* MockProducer::send_offsets_to_transaction(
		const std::vector<RdKafka::TopicPartition*>&, const RdKafka::ConsumerGroupMetadata*, int)
	{
		return nullptr;
	}

	RdKafka::Error* MockProducer::commit_transaction(int)
	{
		return nullptr;
	}

	RdKafka::Error* MockProducer::abort_transaction(int)
	{
		return nullptr;
	}
} // namespace Kafka1C