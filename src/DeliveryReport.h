#pragma once

#include <map>
#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include "Loger.h"

class DeliveryReport : public RdKafka::DeliveryReportCb 
{

public:
	DeliveryReport(Loger* Loger);

	void dr_cb(RdKafka::Message& Message);

	void ClearStatuses();
	RdKafka::Message::Status GetStatus(std::string Uuid);

private:
	Loger* loger;
	std::map<std::string, RdKafka::Message::Status> statuses;

	void LogMessageStatus(RdKafka::Message& Message);
	void AddMessageStatus(RdKafka::Message& Message);

};