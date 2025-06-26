#pragma once

#include <map>
#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include "Loger.h"

namespace Kafka1C {

	class DeliveryReport : public RdKafka::DeliveryReportCb {

	public:
		DeliveryReport(Loger* Loger);

		void dr_cb(RdKafka::Message& Message);

		void ClearStatuses();
		const std::string* AddEmptyStatus(std::string MessageId);
		RdKafka::Message::Status GetStatus(std::string MessageId);
		int CountUndelivered();

	private:
		Loger* loger;
		std::map<std::string, RdKafka::Message::Status> statuses;

		void LogMessageStatus(RdKafka::Message& Message);
		std::string ExtractMessageId(RdKafka::Message& Message);
		void SetStatus(std::string MessageId, RdKafka::Message::Status Status);
	};

} // namespace RdKafka1C